/********************************************************************************
 *
 * FILE: 01_extract_items_and_revisions.cpp
 *
 * PURPOSE:
 *   This utility extracts metadata for Items and Item Revisions from Teamcenter.
 *   It serves as the C++ implementation for the placeholder script
 *   `etl/extract/01_extract_items_and_revisions.bat`.
 *
 * USAGE:
 *   01_extract_items_and_revisions.exe -u=<user> -p=<password> -g=<group> -props=<properties_file> -output=<output_file>
 *
 * INPUT:
 *   - Teamcenter credentials (user, password, group).
 *   - A properties file containing query criteria (e.g., item type, status).
 *   - Path for the output CSV file.
 *
 * OUTPUT:
 *   - A CSV file containing the extracted metadata with the following header:
 *     item_id,object_type,item_name,item_rev_id,rev_object_type,rev_name,rev_status,creation_date,owning_user,owning_group
 *
 * ITK FUNCTIONS USED:
 *   - ITK_init_module()
 *   - ITK_auto_login() / POM_login()
 *   - QRY_find2()
 *   - AOM_ask_value_string()
 *   - ITEM_list_all_revs()
 *   - AOM_UIF_ask_value() / AOM_ask_value_string()
 *   - ITK_exit_module()
 *
 ********************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Standard ITK headers
#include <itk/libitk.h>
#include <pom/pom/pom.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <qry/qry.h>

// Utility function to check ITK return codes
static void ITK_CHECK(int stat) {
    if (stat != ITK_ok) {
        char* error_text = NULL;
        EMH_ask_error_text(stat, &error_text);
        std.cerr << "ITK ERROR: " << stat << " - " << error_text << std.endl;
        if (error_text) MEM_free(error_text);
        exit(stat);
    }
}

// Main extraction logic
void do_it() {
    // --- 1. Get arguments from command line ---
    char* user = ITK_ask_cli_argument("-u=");
    char* pass = ITK_ask_cli_argument("-p=");
    char* group = ITK_ask_cli_argument("-g=");
    char* output_file_path = ITK_ask_cli_argument("-output=");

    if (!user || !pass || !group || !output_file_path) {
        std.cerr << "USAGE: 01_extract_items_and_revisions.exe -u=<user> -p=<password> -g=<group> -output=<output_file>" << std.endl;
        return;
    }

    std.ofstream output_file(output_file_path);
    if (!output_file.is_open()) {
        std.cerr << "ERROR: Could not open output file: " << output_file_path << std::endl;
        return;
    }

    // --- 2. Login to Teamcenter ---
    ITK_CHECK(ITK_auto_login());
    std.cout << "Login successful." << std::endl;

    // --- 3. Find Query and Execute ---
    tag_t query_tag = NULLTAG;
    ITK_CHECK(QRY_find2("Item...", &query_tag)); // A common query name

    char* query_entries[] = {"Type", "Item ID"};
    char* query_values[] = {"Item", "*"}; // Example: Find all Items with any ID

    int num_results = 0;
    tag_t* results = NULL;
    ITK_CHECK(QRY_execute(query_tag, 2, query_entries, query_values, &num_results, &results));
    std.cout << "Found " << num_results << " items." << std::endl;

    // --- 4. Write CSV Header ---
    output_file << "item_id,object_type,item_name,item_rev_id,rev_object_type,rev_name,rev_status,creation_date,owning_user,owning_group" << std::endl;

    // --- 5. Process each item ---
    for (int i = 0; i < num_results; i++) {
        tag_t item_tag = results[i];
        char *item_id = NULL, *item_name = NULL, *object_type = NULL;

        ITK_CHECK(AOM_ask_value_string(item_tag, "item_id", &item_id));
        ITK_CHECK(AOM_ask_value_string(item_tag, "object_name", &item_name));
        ITK_CHECK(AOM_ask_value_string(item_tag, "object_type", &object_type));

        // --- 6. Get all revisions for the item ---
        int rev_count = 0;
        tag_t* rev_tags = NULL;
        ITK_CHECK(ITEM_list_all_revs(item_tag, &rev_count, &rev_tags));

        for (int j = 0; j < rev_count; j++) {
            tag_t rev_tag = rev_tags[j];
            char *rev_id = NULL, *rev_name = NULL, *rev_status = NULL, *rev_obj_type = NULL;
            char *creation_date = NULL, *owning_user = NULL, *owning_group = NULL;

            ITK_CHECK(AOM_ask_value_string(rev_tag, "item_revision_id", &rev_id));
            ITK_CHECK(AOM_ask_value_string(rev_tag, "object_name", &rev_name));
            ITK_CHECK(AOM_ask_value_string(rev_tag, "object_type", &rev_obj_type));

            // Get status
            tag_t status_tag = NULLTAG;
            char* status_string = NULL;
            if (AOM_ask_value_tag(rev_tag, "release_status_list", &status_tag) == ITK_ok && status_tag != NULLTAG) {
                 AOM_ask_value_string(status_tag, "name", &status_string);
            } else {
                status_string = (char*)MEM_alloc(sizeof(char));
                strcpy(status_string, "");
            }

            // Get other properties
            AOM_ask_value_string(rev_tag, "creation_date", &creation_date);
            AOM_ask_value_string(rev_tag, "owning_user", &owning_user);
            AOM_ask_value_string(rev_tag, "owning_group", &owning_group);

            // --- 7. Write to CSV ---
            output_file << "\"" << item_id << "\","
                        << "\"" << object_type << "\","
                        << "\"" << item_name << "\","
                        << "\"" << rev_id << "\","
                        << "\"" << rev_obj_type << "\","
                        << "\"" << rev_name << "\","
                        << "\"" << (status_string ? status_string : "") << "\","
                        << "\"" << creation_date << "\","
                        << "\"" << owning_user << "\","
                        << "\"" << owning_group << "\"" << std::endl;

            // Free memory
            if (rev_id) MEM_free(rev_id);
            if (rev_name) MEM_free(rev_name);
            if (rev_obj_type) MEM_free(rev_obj_type);
            if (status_string) MEM_free(status_string);
            if (creation_date) MEM_free(creation_date);
            if (owning_user) MEM_free(owning_user);
            if (owning_group) MEM_free(owning_group);
        }

        if (item_id) MEM_free(item_id);
        if (item_name) MEM_free(item_name);
        if (object_type) MEM_free(object_type);
        if (rev_tags) MEM_free(rev_tags);
    }

    // --- 8. Cleanup ---
    if (results) MEM_free(results);
    output_file.close();
    std.cout << "Extraction complete. Output written to " << output_file_path << std::endl;
}

// ITK entry point
int ITK_user_main(int argc, char* argv[]) {
    ITK_CHECK(ITK_init_module("user", "password", "group")); // Replace with actual credentials if not using auto_login

    do_it();

    ITK_CHECK(ITK_exit_module());
    return ITK_ok;
}
