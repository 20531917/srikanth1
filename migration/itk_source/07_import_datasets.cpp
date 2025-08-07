/********************************************************************************
 *
 * FILE: 07_import_datasets.cpp
 *
 * PURPOSE:
 *   This utility imports Datasets into Teamcenter from a CSV file and attaches
 *   them to Item Revisions.
 *   Implementation for `etl/load/07_import_datasets.bat`.
 *
 * USAGE:
 *   07_import_datasets.exe -u=<user> -p=<password> -g=<group> -input=<input_csv>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - A CSV file containing dataset and revision information.
 *
 * OUTPUT:
 *   - Datasets created and attached in Teamcenter.
 *
 * ITK FUNCTIONS USED:
 *   - AE_create_dataset()
 *   - GRM_create_relation()
 *   - AOM_save()
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
#include <ae/ae.h>
#include <grm/grm.h>

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
    if (!input_file_path) {
        std::cerr << "USAGE: 07_import_datasets.exe -input=<input.csv>" << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    std::ifstream input_file(input_file_path);
    std::string line;
    std::getline(input_file, line); // Skip header

    while (std::getline(input_file, line)) {
        std::vector<std::string> row = split_csv_line(line);
        if (row.size() < 4) continue;

        std::string item_rev_id = row[0];
        std::string dataset_name = row[1];
        std::string dataset_type = row[2];
        std::string relation_type = row[3];

        tag_t rev_tag = NULLTAG;
        ITK_CHECK(ITEM_find_rev(item_rev_id.c_str(), &rev_tag));
        if (rev_tag == NULLTAG) {
            std::cerr << "ERROR: Could not find revision: " << item_rev_id << std::endl;
            continue;
        }

        tag_t dataset_tag = NULLTAG;
        ITK_CHECK(AE_find_dataset(dataset_name.c_str(), &dataset_tag));

        if (dataset_tag == NULLTAG) {
            std::cout << "Creating dataset: " << dataset_name << std::endl;
            ITK_CHECK(AE_create_dataset(dataset_type.c_str(), dataset_name.c_str(), "Dataset", &dataset_tag));
            ITK_CHECK(AOM_save(dataset_tag));
        }

        // Attach dataset to revision
        tag_t relation_type_tag = NULLTAG, relation_tag = NULLTAG;
        ITK_CHECK(GRM_find_relation_type(relation_type.c_str(), &relation_type_tag));

        if (relation_type_tag != NULLTAG) {
            ITK_CHECK(GRM_create_relation(rev_tag, dataset_tag, relation_type_tag, NULLTAG, &relation_tag));
            ITK_CHECK(AOM_save(relation_tag));
            std::cout << "Attached " << dataset_name << " to " << item_rev_id << std::endl;
        } else {
            std::cerr << "ERROR: Could not find relation type: " << relation_type << std::endl;
        }
    }
    input_file.close();
    std::cout << "Dataset import complete." << std::endl;
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
