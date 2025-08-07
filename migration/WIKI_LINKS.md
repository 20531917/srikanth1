# Wiki Content: Migration Scripts and Resources

This page provides a quick reference to the key scripts, source code, and queries used in the Teamcenter migration project. Use this content to populate your internal project wiki.

## 1. ITK Utility Source Code

The core logic for data extraction and loading is contained in a set of C++ ITK utilities.

*   **Source Code Location:** [`/migration/itk_source/`](./itk_source/)
*   **Compilation Instructions:** See the [`README.md`](./itk_source/README.md) in the source directory for detailed, Windows-specific compilation steps.

### Key Utilities:
| C++ Source File                                     | Purpose                                    | Corresponding Script                               |
| --------------------------------------------------- | ------------------------------------------ | -------------------------------------------------- |
| `01_extract_items_and_revisions.cpp`                | Extracts Item and Revision metadata.       | `etl/extract/01_extract_items_and_revisions.bat`   |
| `02_extract_bom_structures.cpp`                     | Extracts BOM (Bill of Materials) data.     | `etl/extract/02_extract_bom_structures.bat`        |
| `03_extract_datasets.cpp`                           | Finds all Datasets attached to revisions.  | `etl/extract/03_extract_datasets.bat`              |
| `04_export_physical_files.cpp`                      | Exports physical files from Datasets.      | `etl/extract/04_export_physical_files.bat`         |
| `06_import_items_and_revisions.cpp`                 | Imports Items and Revisions.               | `etl/load/06_import_items_and_revisions.bat`       |
| `07_import_datasets.cpp`                            | Imports and attaches Datasets.             | `etl/load/07_import_datasets.bat`                  |
| `08_import_physical_files.cpp`                      | Imports physical files into Datasets.      | `etl/load/08_import_physical_files.bat`            |
| `09_import_bom_structures.cpp`                      | Imports and assembles BOM structures.      | `etl/load/09_import_bom_structures.bat`            |

---

## 2. SQL Queries

A collection of SQL queries is available for data profiling and validation. These are intended to be run directly against the Teamcenter database (Oracle).

*   **Main Query File:** [`/migration/sql/oracle_queries.sql`](./sql/oracle_queries.sql)
    *   Contains queries for profiling data before migration (e.g., counting object types, checking for data quality issues).
*   **Pre-Migration Checks:** [`/migration/sql/pre_migration_checks.sql`](./sql/pre_migration_checks.sql)
    *   A set of validation queries to run on the source database to establish a baseline.
*   **Post-Migration Validation:** [`/migration/sql/post_migration_validation.sql`](./sql/post_migration_validation.sql)
    *   A set of validation queries to run on the target database to verify the migrated data.

---

## 3. Transformation Logic

Data transformation is handled by a Python script that uses a configurable mapping file.

*   **Transformation Script:** [`/migration/etl/transform/05_transform_metadata.py`](./etl/transform/05_transform_metadata.py)
*   **Mapping Template:** [`/migration/etl/transform/DATA_MAPPING_TEMPLATE.csv`](./etl/transform/DATA_MAPPING_TEMPLATE.csv)

---

## 4. High-Level Planning Documents

*   **Architecture Plan:** [`/migration/planning/ARCHITECTURE_PLAN.md`](./planning/ARCHITECTURE_PLAN.md)
*   **Testing Plan:** [`/migration/planning/TESTING_PLAN.md`](./planning/TESTING_PLAN.md)
*   **Cutover Plan:** [`/migration/planning/CUTOVER_PLAN.md`](./planning/CUTOVER_PLAN.md)
