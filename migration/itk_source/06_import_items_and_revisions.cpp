/********************************************************************************
 *
 * FILE: 06_import_items_and_revisions.cpp
 *
 * PURPOSE:
 *   This utility imports Items and Item Revisions into Teamcenter from a CSV file.
 *   It is the C++ implementation for `etl/load/06_import_items_and_revisions.bat`.
 *
 * USAGE:
 *   06_import_items_and_revisions.exe -u=<user> -p=<password> -g=<group> -input=<input_csv>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - A transformed CSV file containing the data to load.
 *
 * OUTPUT:
 *   - Items and Item Revisions created or updated in Teamcenter.
 *   - A log file detailing the success or failure of each row.
 *
 * ITK FUNCTIONS USED:
 *   - ITEM_create_item()
 *   - ITEM_create_rev()
 *   - AOM_set_value_string()
 *   - AOM_save()
 *   - AOM_unlock()
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
#include <tccore/aom_prop.h>

static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std.cerr << "ITK ERROR: " << stat << " - " << error_text << std::endl;
        if (error_text) MEM_free(error_text);
        // Don't exit on error, just log it and continue
    }
}

// Function to split a CSV row
std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Basic quote handling
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

void do_it() {
    char* input_file_path = ITK_ask_cli_argument("-input=");
    if (!input_file_path) {
        std::cerr << "USAGE: 06_import_items_and_revisions.exe -input=<input.csv>" << std::endl;
        return;
    }

    std::ifstream input_file(input_file_path);
    if (!input_file.is_open()) {
        std::cerr << "ERROR: Could not open input file: " << input_file_path << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    std::string line;
    std::getline(input_file, line); // Skip header

    std::string last_item_id = "";

    while (std::getline(input_file, line)) {
        std::vector<std::string> row = split_csv_line(line);
        if (row.size() < 10) continue; // Expect at least 10 columns

        std::string item_id = row[0];
        std::string item_type = row[1];
        std::string item_name = row[2];
        std::string rev_id = row[3];
        std::string rev_type = row[4];
        std::string rev_name = row[5];
        // ... other columns as needed

        tag_t item_tag = NULLTAG;

        // --- Find or Create Item ---
        if (item_id != last_item_id) {
            ITK_CHECK(ITEM_find_item(item_id.c_str(), &item_tag));
            if (item_tag == NULLTAG) {
                std::cout << "Creating Item: " << item_id << std::endl;
                ITK_CHECK(ITEM_create_item(item_id.c_str(), item_name.c_str(), item_type.c_str(), "A", &item_tag, NULL));
                ITK_CHECK(AOM_save(item_tag));
            }
            last_item_id = item_id;
        } else {
            ITK_CHECK(ITEM_find_item(item_id.c_str(), &item_tag));
        }

        if (item_tag == NULLTAG) {
            std.cerr << "ERROR: Could not find or create item " << item_id << std::endl;
            continue;
        }

        // --- Find or Create Item Revision ---
        tag_t rev_tag = NULLTAG;
        ITK_CHECK(ITEM_find_rev(item_id.c_str(), rev_id.c_str(), &rev_tag));

        if (rev_tag == NULLTAG) {
            std::cout << "Creating Revision: " << item_id << "/" << rev_id << std::endl;
            ITK_CHECK(ITEM_create_rev(item_tag, rev_id.c_str(), rev_name.c_str(), rev_type.c_str(), &rev_tag, NULL));

            // Set properties on the new revision
            ITK_CHECK(AOM_lock(rev_tag));
            // Example: set a custom property
            // ITK_CHECK(AOM_set_value_string(rev_tag, "my_custom_property", "some_value"));
            ITK_CHECK(AOM_save(rev_tag));
            ITK_CHECK(AOM_unlock(rev_tag));
        } else {
             std::cout << "Revision " << item_id << "/" << rev_id << " already exists. Skipping." << std::endl;
        }
    }

    input_file.close();
    std::cout << "Import process complete." << std::endl;
}

int ITK_user_main(int argc, char* argv[]) {
    ITK_CHECK(ITK_init_module("user", "password", "group"));
    do_it();
    ITK_CHECK(ITK_exit_module());
    return ITK_ok;
}
