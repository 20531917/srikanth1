# Teamcenter Migration: Detailed Cutover Plan

## 1. Introduction

This document provides a detailed, step-by-step checklist for the final production cutover for the Teamcenter migration project. The plan is designed to be a script that is followed precisely by the migration team during the go-live weekend. Its purpose is to ensure a smooth, predictable, and verifiable transition to the new Teamcenter environment while minimizing risks and downtime.

**Go-Live Date:** [Date]
**Downtime Window:** [Start Time, e.g., Friday 17:00] to [End Time, e.g., Sunday 19:00]
**Target Timezone:** [Timezone, e.g., UTC]

## 2. Supporting Documentation

This cutover plan relies on several other critical documents. These must be treated as **living documents** and be kept up-to-date throughout the project lifecycle. They must be finalized, reviewed, and readily available to the entire migration team during the cutover weekend.

| Document Name                       | Location/Link                               | Purpose                                                              |
| ----------------------------------- | ------------------------------------------- | -------------------------------------------------------------------- |
| **Data Mapping Specification**      | [Link to Document]                          | The definitive guide for all source-to-target field mappings.        |
| **ITK Utility & Script Guide**      | [Link to Document]                          | Technical guide for all custom migration scripts and utilities.      |
| **Environment Configuration Document** | [Link to Document]                          | Contains all IPs, hostnames, and credentials for all systems.        |
| **UAT Test Plan & Scripts**         | [Link to Document]                          | Detailed test cases to be executed by business users.                |
| **Final UAT Sign-off**              | [Link to Document]                          | Formal approval from the business to proceed with the go-live.       |
| **Communication Plan**              | [Link to Document]                          | Pre-defined notifications for users, stakeholders, and support teams. |

## 3. Key Personnel

| Name          | Role                          | Contact Information |
| ------------- | ----------------------------- | ------------------- |
| [Name]        | Project Manager (PM)          | [Email, Phone]      |
| [Name]        | Technical Lead / Architect    | [Email, Phone]      |
| [Name]        | Database Administrator (DBA)  | [Email, Phone]      |
| [Name]        | System Administrator (SysAdmin) | [Email, Phone]      |
| [Name]        | Business Lead / UAT Lead      | [Email, Phone]      |
| [Name]        | CAD Lead                      | [Email, Phone]      |
| [Name]        | Hypercare Support Lead        | [Email, Phone]      |

---

## 4. Pre-Cutover Activities (T-7 Days to T-1 Day)

*This phase includes all final preparations before the official downtime begins.*

| #   | Task                                                               | Owner     | Due Date  | Status |
| --- | ------------------------------------------------------------------ | --------- | --------- | ------ |
| P-1 | Finalize and freeze all migration code, scripts, and configurations. | Tech Lead | T-7 Days  | [ ]    |
| P-2 | Complete final full-scale migration test run in QA environment.    | All       | T-5 Days  | [ ]    |
| P-3 | Review results of final QA test run; resolve any identified issues. | Tech Lead | T-4 Days  | [ ]    |
| P-4 | Finalize all supporting documentation (Section 2).                 | PM        | T-3 Days  | [ ]    |
| P-5 | Perform production system health checks (Source & Target).         | DBA/Admin | T-2 Days  | [ ]    |
| P-6 | Run `pre_migration_checks.sql` on Source DB; analyze results.      | DBA       | T-1 Day   | [ ]    |
| P-7 | Stage all migration scripts and utilities on the migration server.   | Tech Lead | T-1 Day   | [ ]    |
| P-8 | Send initial "planned downtime" communication to all users.        | PM        | T-1 Day   | [ ]    |

---

## 5. Cutover Weekend Execution Plan

**All times are in [Timezone].**

### Phase 1: System Shutdown & Final Backup (Friday Evening)

| #   | Task                                                     | Owner      | Start Time | End Time | Status | Notes |
| --- | -------------------------------------------------------- | ---------- | ---------- | -------- | ------ | ----- |
| 1.1 | Send "downtime starting" notification.                   | PM         | 17:00      | 17:15    | [ ]    |       |
| 1.2 | Lock all users from the source Teamcenter system.        | SysAdmin   | 17:15      | 17:30    | [ ]    |       |
| 1.3 | Perform final shutdown of all source system services.  | SysAdmin   | 17:30      | 18:00    | [ ]    |       |
| 1.4 | Take final "gold" backup of source DB and Volumes.       | DBA/Admin  | 18:00      | 22:00    | [ ]    | **CRITICAL: Rollback Point** |

### Phase 2: Data Migration (Saturday)

| #    | Task                                                                | Owner      | Start Time | End Time | Status | Notes                                                                                               |
| ---- | ------------------------------------------------------------------- | ---------- | ---------- | -------- | ------ | --------------------------------------------------------------------------------------------------- |
| **2.1** | **BMIDE & Data Model Deployment**                                   | Tech Lead  | 08:00      | 09:00    | [ ]    | **Target System Only**                                                                              |
| 2.1.1| Deploy final BMIDE template package using `bmide_deploy`.           | Tech Lead  | 08:00      | 08:30    | [ ]    | `bmide_deploy -u=infodba -p=... -g=dba -package=MyTemplate.zip -mode=upgrade`                       |
| 2.1.2| Verify data model changes in the database schema.                   | DBA        | 08:30      | 09:00    | [ ]    | Check for new tables/columns (e.g., `PMyCustomObject`, `pmy_custom_attribute`).                     |
| **2.2** | **Delta Data Extraction**                                           | Tech Lead  | 09:00      | 13:00    | [ ]    | **Source System Only**                                                                              |
| 2.2.1| Generate delta object list using `delta_data_identification.sql`.   | DBA        | 09:00      | 09:30    | [ ]    | The output is the input for the extraction scripts.                                                 |
| 2.2.2| Run `01_extract_items_and_revisions.bat`                              | Tech Lead  | 09:30      | 10:00    | [ ]    | Extracts core Item and Revision metadata.                                                           |
| 2.2.3| Run `02_extract_bom_structures.bat`                                 | Tech Lead  | 10:00      | 10:30    | [ ]    | Extracts BOM data (PSBOMView, occurrences).                                                         |
| 2.2.4| Run `03_extract_datasets.bat`                                       | Tech Lead  | 10:30      | 11:00    | [ ]    | Extracts Dataset metadata.                                                                          |
| 2.2.5| Run `04_export_physical_files.bat`                                  | Tech Lead  | 11:00      | 11:30    | [ ]    | Exports physical files from volumes using FMS.                                                      |
| 2.2.6| Run CAD Export Utilities (as needed)                                | CAD Lead   | 11:30      | 12:30    | [ ]    | e.g., `nx_clone_export`, `catia_export_tool`, SolidWorks `export_utility`.                          |
| 2.2.7| Run `11_extract_workflows.bat`                                      | Tech Lead  | 12:30      | 13:00    | [ ]    | Extracts running and completed workflow processes.                                                  |
| 2.2.8| Run `13_extract_organization.bat`                                   | Tech Lead  | 13:00      | 13:15    | [ ]    | Extracts Users, Groups, Roles.                                                                      |
| 2.2.9| Run `15_extract_projects.bat`                                       | Tech Lead  | 13:15      | 13:30    | [ ]    | Extracts Teamcenter Project data.                                                                   |
| **2.3** | **Data Transformation**                                             | Tech Lead  | 13:30      | 15:30    | [ ]    | **Migration Server**                                                                                |
| 2.3.1| Run `05_transform_metadata.py` script.                              | Tech Lead  | 13:30      | 15:30    | [ ]    | Applies data mapping rules from the specification.                                                  |
| **2.4** | **Data Loading**                                                    | Tech Lead  | 15:30      | 23:00    | [ ]    | **Target System Only**                                                                              |
| 2.4.1| Run `14_load_organization.bat`                                      | Tech Lead  | 15:30      | 16:00    | [ ]    | **Must be run first.**                                                                              |
| 2.4.2| Run `16_load_projects.bat`                                          | Tech Lead  | 16:00      | 16:30    | [ ]    |                                                                                                     |
| 2.4.3| Run `06_import_items_and_revisions.bat`                             | Tech Lead  | 16:30      | 18:00    | [ ]    | Check logs for errors after completion.                                                             |
| 2.4.4| Run `07_import_datasets.bat`                                        | Tech Lead  | 18:00      | 19:00    | [ ]    |                                                                                                     |
| 2.4.5| Run `08_import_physical_files.bat`                                  | Tech Lead  | 19:00      | 20:00    | [ ]    | This step populates the target volumes.                                                             |
| 2.4.6| Run `09_import_bom_structures.bat`                                  | Tech Lead  | 20:00      | 21:00    | [ ]    |                                                                                                     |
| 2.4.7| Run CAD Import Utilities (as needed)                                | CAD Lead   | 21:00      | 22:00    | [ ]    | e.g., `nx_clone_import`.                                                                            |
| 2.4.8| Run `12_load_workflows.bat`                                         | Tech Lead  | 22:00      | 22:30    | [ ]    | Imports workflow templates and data.                                                                |
| 2.4.9| Install custom workflow handlers.                                   | Tech Lead  | 22:30      | 23:00    | [ ]    | Register custom libraries/dlls required for workflows.                                              |

### Phase 3: System Validation (Sunday Morning)

| #    | Task                                                                | Owner      | Start Time | End Time | Status | Notes                                                                                               |
| ---- | ------------------------------------------------------------------- | ---------- | ---------- | -------- | ------ | --------------------------------------------------------------------------------------------------- |
| **3.1** | **Post-Load Technical Validation**                                  | DBA        | 08:00      | 10:00    | [ ]    |                                                                                                     |
| 3.1.1| Run `post_migration_validation.sql` on Target DB.                   | DBA        | 08:00      | 09:00    | [ ]    |                                                                                                     |
| 3.1.2| Compare results against `pre_migration_checks.sql` output.          | Tech Lead  | 09:00      | 10:00    | [ ]    | Key object counts should match. Integrity checks should return 0 rows.                              |
| **3.2** | **System Smoke Tests**                                              | Tech Team  | 10:00      | 12:00    | [ ]    |                                                                                                     |
| 3.2.1| Test user login (key user types).                                   | Tech Team  | 10:00      | 10:30    | [ ]    |                                                                                                     |
| 3.2.2| Test creating and revising a new Item.                              | Tech Team  | 10:30      | 11:00    | [ ]    |                                                                                                     |
| 3.2.3| Test uploading and downloading a file to a Dataset.                 | Tech Team  | 11:00      | 11:30    | [ ]    | Verifies FMS is working.                                                                            |
| 3.2.4| Test starting a common workflow.                                    | Tech Team  | 11:30      | 12:00    | [ ]    |                                                                                                     |
| **3.3** | **User Acceptance Testing (UAT)**                                   | Business   | 12:00      | 16:00    | [ ]    |                                                                                                     |
| 3.3.1| Key business users execute pre-defined UAT test scripts.            | Business   | 12:00      | 16:00    | [ ]    | UAT script should cover critical business processes on migrated data.                               |

### Phase 4: Go-Live (Sunday Evening)

| #   | Task                                                     | Owner      | Start Time | End Time | Status | Notes |
| --- | -------------------------------------------------------- | ---------- | ---------- | -------- | ------ | ----- |
| 4.1 | **GO/NO-GO DECISION**                                    | All        | 16:00      | 17:00    | [ ] GO | All stakeholders must agree. |
| 4.2 | Re-configure DNS / network to point to new servers.      | SysAdmin   | 17:00      | 18:00    | [ ]    | If GO decision is made. |
| 4.3 | Perform final smoke test using the public URL.           | Tech Lead  | 18:00      | 18:30    | [ ]    |       |
| 4.4 | Send "Go-Live" notification to all users.                | PM         | 18:30      | 19:00    | [ ]    | Include new URL and instructions. |

---

## 6. Post-Go-Live Activities (First Week)

| #   | Task                                                     | Owner      | Start Time | Status    |
| --- | -------------------------------------------------------- | ---------- | ---------- | --------- |
| 5.1 | Hypercare support begins.                                | All        | Mon 08:00  | [ ] Ongoing |
| 5.2 | Daily issue triage meetings.                             | PM         | Daily 09:00 | [ ]       |
| 5.3 | Decommission old production servers.                     | SysAdmin   | TBD        | [ ]       |

---

## 7. Rollback Plan

**A GO/NO-GO decision will be made at [Sunday 16:00]. If a "NO-GO" is declared, the following steps will be taken immediately:**

1.  **Halt all migration activities.** All work on the new system stops.
2.  **Do not re-configure DNS.** Leave all network traffic pointing to the old system.
3.  **Ensure the old production system is offline.** If it was fully shut down, keep it offline.
4.  **Restore the source production database from the "gold" backup taken on Friday.** (See step 1.4).
5.  **Restore the source production volumes from the "gold" backup.**
6.  **Restart all source system services and unlock user access.**
7.  **Perform a smoke test on the restored source system.**
8.  **Communicate to all stakeholders and users that the migration has been postponed.**
9.  **Schedule a root cause analysis meeting for the following day.**
