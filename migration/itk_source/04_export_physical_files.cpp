/********************************************************************************
 *
 * FILE: 04_export_physical_files.cpp
 *
 * PURPOSE:
 *   This utility exports the physical files (named references) from a list of
 *   datasets provided in a CSV file.
 *   Implementation for `etl/extract/04_export_physical_files.bat`.
 *
 * USAGE:
 *   04_export_physical_files.exe -u=<user> -p=<password> -g=<group> -input=<input_csv> -output_dir=<dir>
 *
 * INPUT:
 *   - Teamcenter credentials.
 *   - A CSV file containing dataset names to export.
 *   - An output directory where the files will be saved.
 *
 * OUTPUT:
 *   - Physical files exported from Teamcenter to the specified directory.
 *
 * ITK FUNCTIONS USED:
 *   - AE_find_dataset()
 *   - AE_ask_all_named_refs()
 *   - IMF_export_file()
 *
 ********************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h> // For directory creation

#include <itk/libitk.h>
#include <pom/pom/pom.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <ae/ae.h>
#include <ae/ae_errors.h>
#include <sa/imf.h>

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
    char* output_dir = ITK_ask_cli_argument("-output_dir=");

    if (!input_file_path || !output_dir) {
        std::cerr << "USAGE: 04_export_physical_files.exe -input=<input.csv> -output_dir=<dir>" << std::endl;
        return;
    }

    // A simple way to create the directory in a cross-platform way.
    // On Windows, you might use _mkdir.
    #if defined(_WIN32)
        _mkdir(output_dir);
    #else
        mkdir(output_dir, 0777);
    #endif

    ITK_CHECK(ITK_auto_login());
    std::cout << "Login successful." << std::endl;

    std::ifstream input_file(input_file_path);
    std::string line;
    std::getline(input_file, line); // Skip header

    while (std::getline(input_file, line)) {
        std::vector<std::string> row = split_csv_line(line);
        if (row.size() < 2) continue;
        std::string dataset_name = row[1];

        tag_t dataset_tag = NULLTAG;
        ITK_CHECK(AE_find_dataset(dataset_name.c_str(), &dataset_tag));

        if (dataset_tag == NULLTAG) {
            std::cerr << "WARNING: Could not find dataset: " << dataset_name << std::endl;
            continue;
        }

        int ref_count = 0;
        tag_t* ref_tags = NULL;
        char** ref_names = NULL;

        ITK_CHECK(AE_ask_all_named_refs(dataset_tag, &ref_count, &ref_names, &ref_tags));

        for (int i = 0; i < ref_count; i++) {
            tag_t file_tag = ref_tags[i];
            char original_file_name[IMF_filename_size_c + 1] = "";

            ITK_CHECK(IMF_ask_original_file_name(file_tag, original_file_name));

            std::string export_path = std::string(output_dir) + "/" + original_file_name;

            std::cout << "Exporting file: " << original_file_name << " to " << export_path << std::endl;
            ITK_CHECK(IMF_export_file(file_tag, export_path.c_str()));
        }

        if (ref_tags) MEM_free(ref_tags);
        if (ref_names) MEM_free(ref_names);
    }

    input_file.close();
    std::cout << "File export complete." << std::endl;
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
