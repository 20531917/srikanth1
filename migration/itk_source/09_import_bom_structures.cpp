/********************************************************************************
 *
 * FILE: 09_import_bom_structures.cpp
 *
 * PURPOSE:
 *   This utility imports BOM structures into Teamcenter from a CSV file.
 *   It assembles BOMs based on a parent-child relationship file.
 *   Implementation for `etl/load/09_import_bom_structures.bat`.
 *
 * USAGE:
 *   09_import_bom_structures.exe -u=<user> -p=<password> -g=<group> -input=<input_csv>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - A CSV file defining parent-child relationships and BOM attributes.
 *
 * OUTPUT:
 *   - BOM structures created in Teamcenter.
 *
 * ITK FUNCTIONS USED:
 *   - ITEM_find_rev()
 *   - PS_create_bom_window()
 *   - PS_ask_window_top_line()
 *   - PS_add_child()
 *   - PS_set_bom_line_notes() / AOM_set_value_string() on BOM line
 *   - PS_save_bom_window()
 *   - PS_close_window()
 *
 ********************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <itk/libitk.h>
#include <pom/pom/pom.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <ps/ps.h>

static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std.cerr << "ITK ERROR: " << stat << " - " << error_text << std::endl;
        if (error_text) MEM_free(error_text);
    }
}

std::vector<std::string> split_csv_line(const std::string& line); // Assume defined elsewhere

void do_it() {
    char* input_file_path = ITK_ask_cli_argument("-input=");
    if (!input_file_path) {
        std::cerr << "USAGE: 09_import_bom_structures.exe -input=<input.csv>" << std::endl;
        return;
    }

    std::ifstream input_file(input_file_path);
    if (!input_file.is_open()) {
        std::cerr << "ERROR: Could not open input file." << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    std::string line;
    std::getline(input_file, line); // Skip header

    while (std::getline(input_file, line)) {
        std::vector<std::string> row = split_csv_line(line);
        if (row.size() < 4) continue;

        std::string parent_rev_id_str = row[0];
        std::string child_rev_id_str = row[1];
        std::string find_no = row[2];
        std::string quantity = row[3];

        tag_t parent_rev_tag = NULLTAG, child_rev_tag = NULLTAG;
        ITK_CHECK(ITEM_find_rev(parent_rev_id_str.c_str(), &parent_rev_tag));
        ITK_CHECK(ITEM_find_rev(child_rev_id_str.c_str(), &child_rev_tag));

        if (parent_rev_tag == NULLTAG || child_rev_tag == NULLTAG) {
            std::cerr << "ERROR: Could not find parent or child revision for line: " << line << std::endl;
            continue;
        }

        // --- BOM Window operations ---
        tag_t bom_window = NULLTAG, top_line = NULLTAG;
        ITK_CHECK(PS_create_bom_window(&bom_window));
        ITK_CHECK(PS_set_window_top_line(bom_window, NULLTAG, parent_rev_tag, NULLTAG, &top_line));

        if (top_line == NULLTAG) {
            std::cerr << "ERROR: Could not get top BOM line for " << parent_rev_id_str << std::endl;
            PS_close_window(bom_window);
            continue;
        }

        // --- Add Child to BOM ---
        tag_t child_item_tag = NULLTAG;
        ITK_CHECK(ITEM_ask_item_of_rev(child_rev_tag, &child_item_tag));

        tag_t new_bom_line = NULLTAG;
        ITK_CHECK(PS_add_child(top_line, child_item_tag, child_rev_tag, NULLTAG, &new_bom_line));

        if (new_bom_line != NULLTAG) {
            // Set properties on the new BOM line
            ITK_CHECK(AOM_lock(new_bom_line));
            if (!find_no.empty()) ITK_CHECK(AOM_set_value_string(new_bom_line, "bl_find_no", find_no.c_str()));
            if (!quantity.empty()) ITK_CHECK(AOM_set_value_string(new_bom_line, "bl_quantity", quantity.c_str()));
            ITK_CHECK(AOM_save(new_bom_line));
            ITK_CHECK(AOM_unlock(new_bom_line));

            std::cout << "Added " << child_rev_id_str << " to " << parent_rev_id_str << std::endl;
        } else {
            std::cerr << "ERROR: Failed to add child " << child_rev_id_str << " to " << parent_rev_id_str << std::endl;
        }

        ITK_CHECK(PS_save_bom_window(bom_window));
        ITK_CHECK(PS_close_window(bom_window));
    }

    input_file.close();
    std::cout << "BOM import process complete." << std::endl;
}

int ITK_user_main(int argc, char* argv[]) {
    ITK_CHECK(ITK_init_module("user", "password", "group"));
    do_it();
    ITK_CHECK(ITK_exit_module());
    return ITK_ok;
}

// Assume this function is also available for this file
std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty() && item.front() == '"') {
            item.erase(0, 1);
            if (!item.empty() && item.back() == '"') {
                item.pop_back();
            }
        }
        result.push_back(item);
    }
    return result;
}
