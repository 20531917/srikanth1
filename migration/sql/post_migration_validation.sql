-- =================================================================================
-- TEAMCENTER MIGRATION: POST-MIGRATION VALIDATION SCRIPT
--
-- PURPOSE:
-- To be run on the TARGET Teamcenter database after a migration load.
-- This script runs queries to validate that the data has been loaded correctly.
-- The output of this script should be compared against the output of the
-- pre-migration checks and the source data profile.
--
-- USAGE:
-- 1. Connect to the target database using a read-only account.
-- 2. Execute this script.
-- 3. Compare the resulting counts with the pre-migration check script results.
--    Counts for major object types (Items, Datasets) should match.
-- 4. Review any queries in the "Data Integrity Checks" section. They should return 0.
-- =================================================================================

-- SECTION 1: DATA COUNT VALIDATION
-- (Compare these counts with the pre-migration report from the source DB)

-- 1.1: Count of all migrated Item types.
SELECT 'VALIDATE: Count by Item Type' AS "Check Name", pobject_type, COUNT(*)
FROM PITEM
GROUP BY pobject_type
ORDER BY pobject_type;

-- 1.2: Count of all migrated Dataset types.
SELECT 'VALIDATE: Count by Dataset Type' AS "Check Name", pobject_type, COUNT(*)
FROM PDATASET
GROUP BY pobject_type
ORDER BY pobject_type;

-- 1.3: Count of migrated users.
-- Note: This may not match the source if you are consolidating or cleaning users.
SELECT 'VALIDATE: Total User Count' AS "Check Name", COUNT(*)
FROM PPOM_USER;


-- SECTION 2: DATA INTEGRITY CHECCS
-- (These queries should return 0 rows. Any results indicate a problem.)

-- 2.1: Find Items where a critical custom attribute was not migrated.
-- ACTION: Customize 'pmy_custom_attribute' to a real, critical attribute name from your data model.
-- This helps validate that the data transformation and loading worked correctly.
SELECT 'ERROR: Items with missing critical attribute' AS "Check Name", pitem_id, pobject_type
FROM PITEM
WHERE pmy_custom_attribute IS NULL;

-- 2.2: Validate ownership was set correctly.
-- REASON: Incorrect ownership can prevent users from accessing their data.
-- ACTION: This query checks for objects not owned by a valid user/group. This may need
-- to be customized depending on your ownership migration strategy.
SELECT 'ERROR: Objects with invalid ownership' AS "Check Name", puid, pobject_type
FROM PWORKSPACEOBJECT
WHERE powning_useru IS NULL OR powning_groupu IS NULL;

-- 2.3: Check for broken relationships (IMANRELATION pointing to a non-existent object).
-- REASON: This indicates a failure during the relationship import process.
-- ACTION: This is a complex query and may need adjustment for your specific DB.
SELECT 'ERROR: Broken primary relationships' AS "Check Name", r.puid
FROM PIMANRELATION r
LEFT JOIN PWORKSPACEOBJECT w ON r.rprimary_objectu = w.puid
WHERE w.puid IS NULL;

SELECT 'ERROR: Broken secondary relationships' AS "Check Name", r.puid
FROM PIMANRELATION r
LEFT JOIN PWORKSPACEOBJECT w ON r.rsecondary_objectu = w.puid
WHERE w.puid IS NULL;
