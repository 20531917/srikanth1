# Teamcenter Migration Project: Comprehensive Framework

## 1. Overview

Welcome to the comprehensive framework for the Teamcenter migration project. This document serves as the main entry point for all project-related artifacts, including planning documents, ETL scripts, and technical resources.

The goal of this project is to provide a robust, detailed, and organized template for migrating data from a source Teamcenter environment to a new target environment.

## 2. Project Structure

The project is organized into the following directories:

*   `/planning`: Contains all high-level planning and strategy documents.
*   `/etl`: Contains all the executable code for the migration (Extract, Transform, Load).
*   `/sql`: Contains SQL queries for database analysis and validation.

---

## 3. Planning and Strategy

High-level planning documents define the strategy, scope, and execution plan for the migration.

*   **[Architecture Plan](./planning/ARCHITECTURE_PLAN.md):** Details the server, network, and software architecture for the new Teamcenter environment.
*   **[Cutover Plan](./planning/CUTOVER_PLAN.md):** Contains the detailed, step-by-step checklist for the go-live weekend.
*   **[Testing Plan](./planning/TESTING_PLAN.md):** Describes the strategy for all testing phases, from unit testing to UAT and performance testing.
*   **[Error Handling Strategy](./planning/ERROR_HANDLING_STRATEGY.md):** Defines the process for logging, categorizing, and reprocessing errors.

---

## 4. Data Migration (ETL) Process

The migration is executed via a series of Extract, Transform, and Load (ETL) scripts. These are designed to be run in a specific order.

### 4.1. Pre-Migration Health Check

Before starting the main ETL process, run the health check utility on the source environment to identify potential data issues.

*   **`etl/00_pre_migration_health_check.bat`**: A placeholder utility to find data quality problems before they become migration errors.

### 4.2. ETL Scripts & Configuration

The core of the migration is a set of utilities that handle the data. Each key script can be parameterized using a corresponding properties file in `etl/config/`.

**Execution Order:**

1.  **Extract - Item and CAD Data** (`etl/extract/`)
    *   `01_extract_items_and_revisions.bat`
    *   `02_extract_bom_structures.bat`
    *   `03_extract_datasets.bat`
    *   `04_export_physical_files.bat`

2.  **Transform** (`etl/transform/`)
    *   **`05_transform_metadata.py`**: A Python script that reads the extracted data, applies mapping rules, and generates transformed files for loading.
    *   **`DATA_MAPPING_TEMPLATE.csv`**: A template for defining the transformation rules (type, attribute, and status mappings).

3.  **Load - Item and CAD Data** (`etl/load/`)
    *   `06_import_items_and_revisions.bat`
    *   `07_import_datasets.bat`
    *   `08_import_physical_files.bat`
    *   `09_import_bom_structures.bat`

4.  **Post-Load Validation** (`etl/load/`)
    *   `10_run_data_validation.bat`: A placeholder for a utility that runs automated checks to verify the migrated data in the target system.

### 4.3. Administrative Data Migration

This data is typically migrated using PLMXML utilities. The scripts are located in `etl/extract/` and `etl/load/`.

*   **Workflows:** `11_extract_workflows.bat`, `12_load_workflows.bat`
*   **Organization:** `13_extract_organization.bat`, `14_load_organization.bat`
*   **Teamcenter Projects:** `15_extract_projects.bat`, `16_load_projects.bat`

---

## 5. Database Queries

For analysis, profiling, and direct validation, a set of SQL queries is provided.

*   **[Oracle SQL Queries](./sql/oracle_queries.sql):** A list of common queries for inspecting the Teamcenter database.
