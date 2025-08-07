# Teamcenter Migration Project: Main Wiki

## 1. Overview

Welcome to the central wiki for the Teamcenter migration project. This document serves as the main entry point for all project-related artifacts, including planning documents, ETL scripts, and technical resources.

The goal of this project is to migrate data from the source Teamcenter environment (vX.X) to the new target environment (vY.Y).

## 2. High-Level Planning

High-level planning documents define the strategy, scope, and execution plan for the migration.

*   **[Architecture Plan](./ARCHITECTURE_PLAN.md):** This document details the server, network, and software architecture for the new Teamcenter environment. It covers hardware, software versions, and the layout for DEV, QA, and PROD.

*   **[Cutover Plan](./CUTOVER_PLAN.md):** This document contains the detailed, step-by-step checklist for the go-live weekend. It includes timelines, responsibilities, and the rollback plan.

## 3. Data Migration (ETL) Process

The migration is executed via a series of Extract, Transform, and Load (ETL) scripts. These are designed to be run in a specific order.

### 3.1. ETL Scripts

The core of the migration is a set of utilities that handle the data. The following scripts are placeholders and templates for the actual migration code.

**Location of all scripts:**
*   Extract Scripts: `./extract/`
*   Transform Scripts: `./transform/`
*   Load Scripts: `./load/`

**Execution Order:**

1.  **Extract - Item and CAD Data**
    *   `01_extract_items_and_revisions.bat`: Extracts core Item/Revision metadata.
    *   `02_extract_bom_structures.bat`: Extracts BOM structures.
    *   `03_extract_datasets.bat`: Extracts dataset metadata (file information).
    *   `04_export_physical_files.bat`: Copies physical files from the volume to a staging area.

2.  **Transform**
    *   `05_transform_metadata.bat`: Applies mapping and business rules to the extracted data. This is a highly custom step.

3.  **Load - Item and CAD Data**
    *   `06_import_items_and_revisions.bat`: Loads the transformed Items/Revisions into the target system.
    *   `07_import_datasets.bat`: Creates and attaches dataset objects.
    *   `08_import_physical_files.bat`: Uploads physical files to the new volume.
    *   `09_import_bom_structures.bat`: Rebuilds the BOM structures.

4.  **Post-Load Validation**
    *   `10_run_data_validation.bat`: Runs automated checks to verify the migrated data.

### 3.2. Administrative Data Migration

This data is typically migrated using PLMXML utilities.

*   **Workflows:**
    *   `11_extract_workflows.bat`
    *   `12_load_workflows.bat`
*   **Organization (Users, Groups, Roles):**
    *   `13_extract_organization.bat`
    *   `14_load_organization.bat`
*   **Teamcenter Projects:**
    *   `15_extract_projects.bat`
    *   `16_load_projects.bat`

## 4. Database Queries

For analysis, profiling, and direct validation, a set of SQL queries is provided. These should be used with caution and are primarily for reporting and validation, not for data extraction.

*   **[Oracle SQL Queries](./oracle_queries.sql):** A list of common queries for inspecting the Teamcenter database.

## 5. Data Mapping Specification

**(This is a document you must create and maintain separately, often as an Excel spreadsheet.)**

A critical component of any migration is the Data Mapping document. This file specifies, field by field, how data from the source system maps to the target system. It is the primary input for the `05_transform_metadata` step.

**Example Mappings:**
*   **Type Mappings:** `OldPart` (source) -> `CompanyStdPart` (target)
*   **Attribute Mappings:** `old_attr_name` (source) -> `new_attr_name` (target)
*   **Status Mappings:** `Released` (source) -> `Production` (target)
*   **Default Values:** Set `new_attr_2` to "Default Value" for all migrated parts.
