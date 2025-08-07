/********************************************************************************
 *
 * FILE: 03_extract_datasets.cpp
 *
 * PURPOSE:
 *   This utility finds all Datasets attached to a given list of Item Revisions.
 *   It is the C++ implementation for `etl/extract/03_extract_datasets.bat`.
 *
 * USAGE:
 *   03_extract_datasets.exe -u=<user> -p=<password> -g=<group> -input=<input_csv> -output=<output_csv>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - An input CSV file containing item revision IDs.
 *   - Path for the output CSV file.
 *
 * OUTPUT:
 *   - A CSV file listing the datasets found, their type, and the revision
 *     they are attached to.
 *     Header: item_rev_id,dataset_name,dataset_type,relation_type
 *
 * ITK FUNCTIONS USED:
 *   - ITEM_find_rev()
 *   - GRM_list_secondary_objects_only()
 *   - AOM_ask_value_string()
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
#include <tccore/grm.h>
#include <tccore/tcaehelper.h>

static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std.cerr << "ITK ERROR: " << stat << " - " << error_text << std::endl;
        if (error_text) MEM_free(error_text);
    }
}

std::vector<std::string> split_csv_line(const std::string& line); // Assume defined

void do_it() {
    char* input_file_path = ITK_ask_cli_argument("-input=");
    char* output_file_path = ITK_ask_cli_argument("-output=");

    if (!input_file_path || !output_file_path) {
        std::cerr << "USAGE: 03_extract_datasets.exe -input=<input.csv> -output=<output.csv>" << std::endl;
        return;
    }

    std::ifstream input_file(input_file_path);
    std::ofstream output_file(output_file_path);

    if (!input_file.is_open() || !output_file.is_open()) {
        std::cerr << "ERROR: Could not open input or output file." << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    output_file << "item_rev_id,dataset_name,dataset_type,relation_type" << std::endl;

    std::string line;
    std::getline(input_file, line); // Skip header

    while (std::getline(input_file, line)) {
        std.vector<std::string> row = split_csv_line(line);
        if (row.size() < 4) continue;
        std::string item_rev_id = row[3];

        tag_t rev_tag = NULLTAG;
        ITK_CHECK(ITEM_find_rev(item_rev_id.c_str(), &rev_tag));
        if (rev_tag == NULLTAG) continue;

        int rel_count = 0;
        tag_t* relations = NULL;
        tag_t* secondary_objects = NULL;

        // Find all objects attached to the revision
        ITK_CHECK(GRM_list_secondary_objects_only(rev_tag, NULLTAG, &rel_count, &relations, &secondary_objects));

        for (int i = 0; i < rel_count; i++) {
            char* obj_type = NULL;
            ITK_CHECK(AOM_ask_value_string(secondary_objects[i], "object_type", &obj_type));

            // We only care about Datasets
            if (strcmp(obj_type, "Dataset") == 0) {
                char *dataset_name = NULL, *dataset_type = NULL, *rel_type_name = NULL;
                tag_t rel_type_tag = NULLTAG;

                ITK_CHECK(AOM_ask_value_string(secondary_objects[i], "object_name", &dataset_name));
                ITK_CHECK(AOM_ask_value_string(secondary_objects[i], "object_type", &dataset_type));
                ITK_CHECK(GRM_ask_relation_type(relations[i], &rel_type_tag));
                ITK_CHECK(AOM_ask_value_string(rel_type_tag, "object_name", &rel_type_name));

                output_file << "\"" << item_rev_id << "\","
                            << "\"" << dataset_name << "\","
                            << "\"" << dataset_type << "\","
                            << "\"" << rel_type_name << "\"" << std::endl;

                if (dataset_name) MEM_free(dataset_name);
                if (dataset_type) MEM_free(dataset_type);
                if (rel_type_name) MEM_free(rel_type_name);
            }
            if (obj_type) MEM_free(obj_type);
        }
        if (relations) MEM_free(relations);
        if (secondary_objects) MEM_free(secondary_objects);
    }

    input_file.close();
    output_file.close();
    std::cout << "Dataset extraction complete." << std::endl;
}

int ITK_user_main(int argc, char* argv[]) {
    ITK_CHECK(ITK_init_module("user", "password", "group"));
    do_it();
    ITK_CHECK(ITK_exit_module());
    return ITK_ok;
}

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
