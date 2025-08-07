# -*- coding: utf-8 -*-
"""
05. Transform Metadata

This script reads the extracted data (e.g., from item_revisions.csv),
applies transformation rules defined in a mapping file, and writes the
output to a new set of files ready for loading.

This is a more realistic implementation of the transformation step.
"""
import pandas as pd
import argparse
import os
import logging

# --- Setup Logging ---
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s] [%(levelname)s] - %(message)s'
)

def load_mappings(mapping_file):
    """
    Loads all mappings from the mapping CSV file into a dictionary of DataFrames.
    """
    mappings = {}
    try:
        with open(mapping_file, 'r') as f:
            for line in f:
                if line.strip().startswith('#SECTION'):
                    section = line.strip().split(',')[1]
                    mappings[section] = pd.read_csv(f, skip_blank_lines=True, comment='#')
    except FileNotFoundError:
        logging.error(f"Mapping file not found: {mapping_file}")
        raise
    return mappings

def main():
    """ Main function to execute the transformation process. """
    parser = argparse.ArgumentParser(description="Teamcenter Data Transformation Script")
    parser.add_argument("--input_file", required=True, help="Path to the input CSV file to transform.")
    parser.add_argument("--output_file", required=True, help="Path to write the transformed CSV file.")
    parser.add_argument("--mapping_file", required=True, help="Path to the data mapping CSV file.")
    args = parser.parse_args()

    logging.info("--- Starting Transformation Process ---")
    logging.info(f"Input file: {args.input_file}")
    logging.info(f"Mapping file: {args.mapping_file}")

    # --- Load Data and Mappings ---
    try:
        df = pd.read_csv(args.input_file)
        mappings = load_mappings(args.mapping_file)
        logging.info("Successfully loaded data and mappings.")
    except Exception as e:
        logging.error(f"Failed to load data or mappings. Error: {e}")
        return

    # --- Apply Transformations ---

    # 1. Type Mapping
    if 'TYPE_MAPPING' in mappings:
        type_map = mappings['TYPE_MAPPING'].set_index('SourceType')['TargetType'].to_dict()
        df['pobject_type'] = df['pobject_type'].map(type_map).fillna(df['pobject_type'])
        logging.info("Applied type mappings.")

    # 2. Status Value Mapping
    if 'STATUS_VALUE_MAPPING' in mappings:
        status_map = mappings['STATUS_VALUE_MAPPING'].set_index('SourceStatus')['TargetStatus'].to_dict()
        # Assume status attribute is named 'pstatus'
        if 'pstatus' in df.columns:
            df['pstatus'] = df['pstatus'].map(status_map).fillna(df['pstatus'])
            logging.info("Applied status value mappings.")

    # 3. Attribute Renaming
    if 'ATTRIBUTE_MAPPING' in mappings:
        attr_map = mappings['ATTRIBUTE_MAPPING'].set_index('SourceAttribute')['TargetAttribute'].to_dict()
        df.rename(columns=attr_map, inplace=True)
        logging.info("Applied attribute renaming.")

    # 4. Default Values (Example)
    if 'DEFAULT_VALUES' in mappings:
        for index, row in mappings['DEFAULT_VALUES'].iterrows():
            target_type = row['TargetType']
            target_attr = row['TargetAttribute']
            default_val = row['Value']
            # Apply default value only where the target attribute does not exist or is null
            if target_attr not in df.columns:
                df[target_attr] = None
            df.loc[(df['object_type'] == target_type) & (df[target_attr].isnull()), target_attr] = default_val
        logging.info("Applied default values.")

    # --- Save Transformed Data ---
    try:
        os.makedirs(os.path.dirname(args.output_file), exist_ok=True)
        df.to_csv(args.output_file, index=False)
        logging.info(f"Successfully saved transformed data to {args.output_file}")
    except Exception as e:
        logging.error(f"Failed to save output file. Error: {e}")

    logging.info("--- Transformation Process Finished ---")

if __name__ == "__main__":
    # Example Usage:
    # python 05_transform_metadata.py --input_file ../output/extracted/item_revisions.csv --output_file ../output/transformed/item_revisions_transformed.csv --mapping_file ./DATA_MAPPING_TEMPLATE.csv
    main()
