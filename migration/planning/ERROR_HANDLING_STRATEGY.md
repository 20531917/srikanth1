# Teamcenter Migration: Error Handling and Logging Strategy

## 1. Introduction

This document defines the strategy for logging, monitoring, and handling errors that occur during the execution of the ETL scripts. A robust error handling process is critical for ensuring data completeness and for efficiently managing the migration process.

## 2. Logging Standards

All ETL scripts (both batch scripts and the Python transformation script) must adhere to the following logging standards.

### 2.1. Log Levels

*   **INFO:** General process information (e.g., "Starting extraction of 100 items", "Script completed successfully").
*   **WARN:** Non-critical issues that did not stop the process but should be reviewed (e.g., "Attribute 'x' was not found for item '123', skipping attribute").
*   **ERROR:** A critical issue that prevented a specific object from being processed (e.g., "Failed to create item '456' due to invalid type"). The script should log the error and continue with the next object.
*   **FATAL:** A critical issue that prevents the entire script from continuing (e.g., "Cannot connect to the database"). The script should terminate.

### 2.2. Log Format

Each log entry should be a single line containing:
`[Timestamp] [Log Level] [Script Name] [Object ID] - [Message]`

**Example:**
`[2023-10-27 10:30:15] [ERROR] [06_import_items_and_revisions] [ITEM-00123] - Failed to create item. Reason: Invalid item type 'OldPartType'.`

### 2.3. Log Files

*   Each execution of a script should generate a unique, timestamped log file (e.g., `01_extract_items_20231027-1000.log`).
*   A central "summary" log should be created for each full migration run, consolidating all key statistics and error counts.

## 3. Error Categorization

Errors will be categorized to help with analysis and reprocessing.

*   **Category 1: Source Data Issue:** The error is caused by bad data in the source system (e.g., illegal characters, missing required attributes).
    *   **Action:** Report to the data owners for cleanup in the source system.
*   **Category 2: Transformation Logic Issue:** The error is caused by a bug or a gap in the transformation script (e.g., a type or attribute was not mapped).
    *   **Action:** Fix the transformation script or data mapping file.
*   **Category 3: Load/Environment Issue:** The error is caused by the target environment (e.g., permissions error, license not available, database connection lost).
    *   **Action:** Resolve the issue in the target environment.

## 4. Reprocessing Strategy

It is inefficient to re-run the entire migration for a small number of failures. A strategy for reprocessing only the failed objects is required.

1.  **Generate a "Failed Objects" File:** After each script runs, parse the log file to extract the IDs of all objects that failed to process. Create a file (e.g., `06_import_items_failures.csv`) listing these IDs.
2.  **Modify Scripts to Accept an Input File:** Each ETL script should have an optional command-line argument to accept an input file of object IDs. If this argument is provided, the script should process *only* the objects listed in that file, rather than its default query.
3.  **The Reprocessing Loop:**
    *   Run the main script.
    *   Generate the failure file.
    *   Analyze the errors and fix the root cause (e.g., update the mapping file).
    *   Re-run the same script, but this time passing in the failure file as input.
    *   Repeat until the failure file is empty.
