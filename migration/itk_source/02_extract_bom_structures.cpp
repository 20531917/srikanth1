/********************************************************************************
 *
 * FILE: 02_extract_bom_structures.cpp
 *
 * PURPOSE:
 *   This utility extracts Bill of Materials (BOM) structure for a given set of
 *   Item Revisions. It traverses the BOM and writes the parent-child relationships.
 *   Implementation for `etl/extract/02_extract_bom_structures.bat`.
 *
 * USAGE:
 *   02_extract_bom_structures.exe -u=<user> -p=<password> -g=<group> -input=<input_csv> -output=<output_csv>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - An input CSV file containing a list of item revision IDs to process.
 *   - Path for the output CSV file.
 *
 * OUTPUT:
 *   - A CSV file mapping parent to child revisions with BOM attributes.
 *     Header: parent_rev_id,child_rev_id,find_no,quantity
 *
 * ITK FUNCTIONS USED:
 *   - PS_ask_bom_view_tags()
 *   - PS_create_bom_window()
 *   - PS_set_rev_rule_for_window()
 *   - PS_ask_window_top_line()
 *   - PS_ask_child_lines()
 *   - AOM_ask_value_string() on BOM line attributes
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
#include <tccore/tcaehelper.h>

static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std.cerr << "ITK ERROR: " << stat << " - " << error_text << std.endl;
        if (error_text) MEM_free(error_text);
        exit(stat);
    }
}

void traverse_bom_recursive(tag_t bom_line, std::ofstream& output_file) {
    char* parent_rev_id = NULL;
    tag_t parent_rev_tag = NULLTAG;
    ITK_CHECK(AOM_ask_value_tag(bom_line, "bl_line_object", &parent_rev_tag));
    ITK_CHECK(AOM_ask_value_string(parent_rev_tag, "item_revision_id", &parent_rev_id));

    int child_count = 0;
    tag_t* child_lines = NULL;
    ITK_CHECK(PS_ask_child_lines(bom_line, &child_count, &child_lines));

    for (int i = 0; i < child_count; i++) {
        tag_t child_line = child_lines[i];
        char *child_rev_id = NULL, *find_no = NULL, *quantity = NULL;
        tag_t child_rev_tag = NULLTAG;

        ITK_CHECK(AOM_ask_value_tag(child_line, "bl_line_object", &child_rev_tag));
        ITK_CHECK(AOM_ask_value_string(child_rev_tag, "item_revision_id", &child_rev_id));
        ITK_CHECK(AOM_ask_value_string(child_line, "bl_find_no", &find_no));
        ITK_CHECK(AOM_ask_value_string(child_line, "bl_quantity", &quantity));

        output_file << "\"" << parent_rev_id << "\","
                    << "\"" << child_rev_id << "\","
                    << "\"" << (find_no ? find_no : "") << "\","
                    << "\"" << (quantity ? quantity : "") << "\"" << std::endl;

        // Recurse to process sub-assemblies
        traverse_bom_recursive(child_line, output_file);

        if (child_rev_id) MEM_free(child_rev_id);
        if (find_no) MEM_free(find_no);
        if (quantity) MEM_free(quantity);
    }

    if (parent_rev_id) MEM_free(parent_rev_id);
    if (child_lines) MEM_free(child_lines);
}

void do_it() {
    char* input_file_path = ITK_ask_cli_argument("-input=");
    char* output_file_path = ITK_ask_cli_argument("-output=");

    if (!input_file_path || !output_file_path) {
        std.cerr << "USAGE: 02_extract_bom_structures.exe -input=<input.csv> -output=<output.csv>" << std::endl;
        return;
    }

    std::ifstream input_file(input_file_path);
    std.ofstream output_file(output_file_path);

    if (!input_file.is_open() || !output_file.is_open()) {
        std.cerr << "ERROR: Could not open input or output file." << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std.cout << "Login successful." << std::endl;

    output_file << "parent_rev_id,child_rev_id,find_no,quantity" << std::endl;

    // --- Process each revision from the input file ---
    std::string line;
    // Skip header
    std::getline(input_file, line);

    while (std::getline(input_file, line)) {
        std::stringstream ss(line);
        std::string item_rev_id;
        // Assuming rev_id is in the 4th column of the items extract
        for(int i=0; i<4; ++i) std::getline(ss, item_rev_id, ',');
        item_rev_id.erase(0, item_rev_id.find_first_not_of(" \t\""));
        item_rev_id.erase(item_rev_id.find_last_not_of(" \t\"") + 1);

        if (item_rev_id.empty()) continue;

        tag_t item_rev_tag = NULLTAG;
        ITK_CHECK(ITEM_find_rev(item_rev_id.c_str(), &item_rev_tag));

        if (item_rev_tag == NULLTAG) {
            std.cerr << "WARNING: Could not find ItemRevision: " << item_rev_id << std::endl;
            continue;
        }

        // --- Create BOM Window ---
        tag_t bom_window = NULLTAG, rule = NULLTAG, bom_view = NULLTAG;
        ITK_CHECK(PS_ask_default_view_type(&bom_view));
        ITK_CHECK(PS_create_bom_window(&bom_window));
        ITK_CHECK(PS_ask_default_rule(&rule));
        ITK_CHECK(PS_set_rev_rule_for_window(rule, bom_window));

        tag_t top_line = NULLTAG;
        ITK_CHECK(PS_set_window_top_line(bom_window, NULLTAG, item_rev_tag, bom_view, &top_line));

        if (top_line != NULLTAG) {
            traverse_bom_recursive(top_line, output_file);
        }

        ITK_CHECK(PS_close_window(bom_window));
    }

    input_file.close();
    output_file.close();
    std.cout << "BOM extraction complete." << std::endl;
}

int ITK_user_main(int argc, char* argv[]) {
    ITK_CHECK(ITK_init_module("user", "password", "group"));
    do_it();
    ITK_CHECK(ITK_exit_module());
    return ITK_ok;
}
