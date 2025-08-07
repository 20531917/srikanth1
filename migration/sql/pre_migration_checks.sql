-- =================================================================================
-- TEAMCENTER MIGRATION: PRE-MIGRATION HEALTH CHECK SCRIPT
--
-- PURPOSE:
-- To be run on the SOURCE Teamcenter database before the final cutover begins.
-- This script identifies common data issues that can cause migration failures
-- and provides a baseline profile of the data to be migrated.
--
-- USAGE:
-- 1. Connect to the source database using a read-only account.
-- 2. Execute this script.
-- 3. Review the output for any red flags (e.g., counts > 0 for problem queries).
-- 4. Address any identified issues in the source system before proceeding with
--    the final data extraction.
-- =================================================================================

-- SECTION 1: CRITICAL PRE-MIGRATION CHECKS
-- (These queries should return a count of 0. Any other result requires action.)

-- 1.1: Find all checked-out objects.
-- REASON: Checked-out objects cannot be migrated and will cause errors.
-- ACTION: Ensure all users have checked in their work before the migration window.
SELECT 'CRITICAL: Found checked-out objects' AS "Check Name", COUNT(*) AS "Result"
FROM PWORKSPACEOBJECT
WHERE pcheckout_switch = 1;

-- 1.2: Find Items with no revisions.
-- REASON: These may be orphaned or incomplete data that needs cleanup.
-- ACTION: Review these items. Decide whether to delete them or create a stub revision.
SELECT 'CRITICAL: Found Items with no revisions' AS "Check Name", COUNT(*) AS "Result"
FROM PITEM pi
WHERE NOT EXISTS (
    SELECT 1 FROM PITEMREVISION pir WHERE pir.ritems_tagu = pi.puid
);

-- 1.3: Find orphaned Item Revisions (revisions without a parent item).
-- REASON: This indicates database integrity issues.
-- ACTION: This is a serious issue that requires investigation by a DBA.
SELECT 'CRITICAL: Found orphaned Item Revisions' AS "Check Name", COUNT(*) AS "Result"
FROM PITEMREVISION pir
WHERE NOT EXISTS (
    SELECT 1 FROM PITEM pi WHERE pi.puid = pir.ritems_tagu
);

-- 1.4: Find datasets not attached to anything.
-- REASON: These are unreferenced files taking up storage.
-- ACTION: Review and decide whether to delete them to save migration time and storage.
SELECT 'WARNING: Found unattached datasets' AS "Check Name", COUNT(*) AS "Result"
FROM PDATASET pd
WHERE NOT EXISTS (
    SELECT 1 FROM PIMANRELATION pr WHERE pr.rsecondary_objectu = pd.puid
);


-- SECTION 2: DATA PROFILING
-- (These queries provide a baseline count of objects to be migrated.)

-- 2.1: Count of all objects by class name.
SELECT 'PROFILE: Count by Class Name' AS "Check Name", pclass_name, COUNT(*)
FROM PWORKSPACEOBJECT
GROUP BY pclass_name
ORDER BY pclass_name;

-- 2.2: Count of all Item types.
SELECT 'PROFILE: Count by Item Type' AS "Check Name", pobject_type, COUNT(*)
FROM PITEM
GROUP BY pobject_type
ORDER BY pobject_type;

-- 2.3: Count of all Dataset types.
SELECT 'PROFILE: Count by Dataset Type' AS "Check Name", pobject_type, COUNT(*)
FROM PDATASET
GROUP BY pobject_type
ORDER BY pobject_type;

-- 2.4: Count of users by their default group.
SELECT 'PROFILE: User Count by Default Group' AS "Check Name", pdefault_group, COUNT(*)
FROM PPOM_USER
GROUP BY pdefault_group
ORDER BY pdefault_group;
