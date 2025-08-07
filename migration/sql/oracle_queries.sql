/*
 * ORACLE SQL QUERIES FOR TEAMCENTER MIGRATION
 *
 * PURPOSE:
 * This file contains a collection of common Oracle SQL queries that are useful
 * during a Teamcenter migration project. These queries are intended for data
 * analysis, profiling, and validation directly against the database.
 *
 * DISCLAIMER:
 * - ALWAYS run these queries in a non-production environment first.
 * - NEVER run "DELETE" or "UPDATE" queries on a production database without
 *   extensive testing and a valid backup.
 * - These queries are provided as a template. You WILL need to customize them
 *   to match your specific data model (e.g., custom object types, custom properties).
 * - Querying the database directly bypasses Teamcenter business logic. For data
 *   extraction for migration, ITK utilities are strongly preferred.
 */

-- =================================================================================
-- SECTION 1: DATA PROFILING QUERIES (Run on SOURCE database)
-- Use these to understand the scope and complexity of your source data.
-- =================================================================================

-- 1.1: Count of all objects by class name
-- Helps to understand the volume of data you are dealing with.
SELECT pclass_name, COUNT(*)
FROM PWORKSPACEOBJECT
GROUP BY pclass_name
ORDER BY pclass_name;

-- 1.2: Count of all Item types
-- Useful for identifying all custom item types and their volumes.
SELECT pobject_type, COUNT(*)
FROM PITEM
GROUP BY pobject_type
ORDER BY pobject_type;

-- 1.3: Count of all Dataset types
-- Useful for identifying all dataset types and planning for file migration.
SELECT pobject_type, COUNT(*)
FROM PDATASET
GROUP BY pobject_type
ORDER BY pobject_type;

-- 1.4: List all users and their default group
-- Helps in planning the organization migration.
SELECT puser_id, pdefault_group
FROM PPOM_USER;

-- 1.5: Find checked-out objects
-- Checked-out objects can cause issues during migration. They should be checked in.
SELECT puid
FROM PWORKSPACEOBJECT
WHERE pcheckout_switch = 1;

-- 1.6: Find Items with no revisions
-- These might be orphaned or incomplete data that needs to be cleaned up.
SELECT pitem_id
FROM PITEM pi
WHERE NOT EXISTS (
    SELECT 1 FROM PITEMREVISION pir WHERE pir.ritems_tagu = pi.puid
);

-- 1.7: Count of Datasets by type and their total file size
-- Helps in planning storage and file transfer strategy.
SELECT
    ds.pobject_type AS dataset_type,
    COUNT(ds.puid) AS dataset_count,
    SUM(f.pfile_size) / 1024 / 1024 AS total_size_mb
FROM
    PDATASET ds
JOIN
    PIMANFILE f ON ds.puid = f.pvolume_tagu
GROUP BY
    ds.pobject_type
ORDER BY
    total_size_mb DESC;

-- 1.8: Find all ImanFile extensions
-- Useful for understanding the types of files stored in the system (e.g., .prt, .asm, .pdf).
SELECT
    SUBSTR(poriginal_file_name, INSTR(poriginal_file_name, '.', -1) + 1) AS file_extension,
    COUNT(*) AS file_count
FROM
    PIMANFILE
WHERE
    poriginal_file_name LIKE '%.%'
GROUP BY
    SUBSTR(poriginal_file_name, INSTR(poriginal_file_name, '.', -1) + 1)
ORDER BY
    file_count DESC;

-- =================================================================================
-- SECTION 2: MIGRATION VALIDATION QUERIES (Run on SOURCE and TARGET databases)
-- Use these to compare data between the source and target systems after a test load.
-- =================================================================================

-- 2.1: Count of migrated Items by type
-- Compare the counts between source and target to ensure all items were created.
SELECT pobject_type, COUNT(*)
FROM PITEM
GROUP BY pobject_type
ORDER BY pobject_type;

-- 2.2: Check for missing attribute values in the target system
-- Replace 'pmy_custom_attribute' with a real attribute name.
-- This helps validate that the data transformation and loading worked correctly.
SELECT pitem_id
FROM PITEM
WHERE pmy_custom_attribute IS NULL;

-- 2.3: Validate ownership
-- Check if the owning user and group were set correctly in the target system.
-- Replace 'Migrated_User' and 'Migrated_Group' with expected values.
SELECT puid
FROM PWORKSPACEOBJECT
WHERE powning_useru NOT IN (SELECT puid FROM PPOM_USER WHERE puser_id = 'Migrated_User')
   OR powning_groupu NOT IN (SELECT puid FROM PGROUP WHERE pgroup_name = 'Migrated_Group');

-- =================================================================================
-- SECTION 3: CLEANUP QUERIES (USE WITH EXTREME CAUTION)
-- These queries are for identifying data to be deleted.
-- =================================================================================

-- 3.1: Find orphaned Item Revisions (revisions without a parent item)
-- This should not happen in a healthy database, but can occur.
SELECT puid
FROM PITEMREVISION pir
WHERE NOT EXISTS (
    SELECT 1 FROM PITEM pi WHERE pi.puid = pir.ritems_tagu
);

-- 3.2: Find datasets not attached to anything
-- These could be candidates for cleanup.
SELECT puid
FROM PDATASET pd
WHERE NOT EXISTS (
    SELECT 1 FROM PIMANRELATION pr WHERE pr.rsecondary_objectu = pd.puid
);
