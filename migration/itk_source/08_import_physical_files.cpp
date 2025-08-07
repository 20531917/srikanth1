/********************************************************************************
 *
 * FILE: 08_import_physical_files.cpp
 *
 * PURPOSE:
 *   This utility imports physical files from a local directory into specified
 *   Teamcenter datasets.
 *   Implementation for `etl/load/08_import_physical_files.bat`.
 *
 * USAGE:
 *   08_import_physical_files.exe -u=<user> -p=<password> -g=<group> -input=<input_csv> -input_dir=<dir>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - A CSV file mapping datasets to file names.
 *   - The local directory where the files are stored.
 *
 * OUTPUT:
 *   - Files imported into Teamcenter and attached to datasets.
 *
 * ITK FUNCTIONS USED:
 *   - AE_find_dataset()
 *   - IMF_import_file()
 *   - AOM_save() on dataset
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
#include <tccore/aom.h>
#include <ae/ae.h>
#include <sa/imf.h>

static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std::cerr << "ITK ERROR: " << stat << " - " << error_text << std::endl;
        if (error_text) MEM_free(error_text);
    }
}

std::vector<std::string> split_csv_line(const std::string& line); // Assume defined

void do_it() {
    char* input_file_path = ITK_ask_cli_argument("-input=");
    char* input_dir = ITK_ask_cli_argument("-input_dir=");

    if (!input_file_path || !input_dir) {
        std::cerr << "USAGE: 08_import_physical_files.exe -input=<input.csv> -input_dir=<dir>" << std::endl;
        return;
    }

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    std::ifstream input_file(input_file_path);
    std::string line;
    std::getline(input_file, line); // Skip header

    while (std::getline(input_file, line)) {
        std::vector<std::string> row = split_csv_line(line);
        // This part needs to be customized based on the actual file format
        // For now, let's assume a simple format: dataset_name,file_name,named_ref
        if (row.size() < 3) continue;

        std::string dataset_name = row[0];
        std::string file_name = row[1];
        std::string named_ref = row[2];

        tag_t dataset_tag = NULLTAG;
        ITK_CHECK(AE_find_dataset(dataset_name.c_str(), &dataset_tag));

        if (dataset_tag == NULLTAG) {
            std::cerr << "ERROR: Could not find dataset: " << dataset_name << std::endl;
            continue;
        }

        std::string full_file_path = std::string(input_dir) + "/" + file_name;

        std::cout << "Importing " << full_file_path << " to dataset " << dataset_name << std::endl;

        tag_t file_tag = NULLTAG;
        ITK_CHECK(IMF_import_file(full_file_path.c_str(), named_ref.c_str(), SS_BINARY, &file_tag, NULL));

        if (file_tag != NULLTAG) {
            ITK_CHECK(AOM_lock(dataset_tag));
            ITK_CHECK(AE_add_named_ref(dataset_tag, named_ref.c_str(), AE_PART_OF, file_tag));
            ITK_CHECK(AOM_save(dataset_tag));
            ITK_CHECK(AOM_unlock(dataset_tag));
            std::cout << "Successfully imported and attached." << std::endl;
        } else {
            std::cerr << "ERROR: Failed to import file: " << full_file_path << std::endl;
        }
    }

    input_file.close();
    std::cout << "File import process complete." << std::endl;
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
