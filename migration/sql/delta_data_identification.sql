-- =================================================================================
-- TEAMCENTER MIGRATION: DELTA DATA IDENTIFICATION SCRIPT
--
-- PURPOSE:
-- To be run on the SOURCE Teamcenter database before the final delta extraction.
-- This script provides queries to identify data that has been created or modified
-- since the last full migration run. This helps to scope the final "delta"
-- migration effort.
--
-- USAGE:
-- 1. Determine the cutoff timestamp from the last migration test run.
--    This is the date/time after which you want to find new or changed data.
-- 2. Replace the placeholder 'YYYY-MM-DD HH24:MI:SS' in the queries below
--    with the actual cutoff timestamp.
-- 3. Execute the script against the source database.
-- 4. The output will be the list of objects that need to be extracted for the
--    final delta migration. The extraction scripts/utilities should be configured
--    to process this list.
-- =================================================================================

-- DEFINE THE CUTOFF TIMESTAMP HERE
-- Example: TO_TIMESTAMP('2023-10-27 18:00:00', 'YYYY-MM-DD HH24:MI:SS')
DEFINE cutoff_timestamp = "TO_TIMESTAMP('YYYY-MM-DD HH24:MI:SS', 'YYYY-MM-DD HH24:MI:SS')";

-- SECTION 1: IDENTIFY NEW OBJECTS
-- (Objects created since the last migration run)

-- 1.1: New Items created since cutoff
SELECT pitem_id FROM PITEM
WHERE pcreation_date > &cutoff_timestamp;

-- 1.2: New Item Revisions created since cutoff
SELECT pitem_id, pitem_revision_id FROM PITEMREVISION
WHERE pcreation_date > &cutoff_timestamp;

-- 1.3: New Datasets created since cutoff
SELECT puid FROM PDATASET
WHERE pcreation_date > &cutoff_timestamp;


-- SECTION 2: IDENTIFY MODIFIED OBJECTS
-- (Objects modified since the last migration run)
-- Note: This is highly dependent on your data model. You must check attributes
-- that users can change. The pdate_modified attribute is a good starting point.

-- 2.1: Item Revisions that have been modified since cutoff
-- (e.g., a status change, an attribute was changed)
SELECT pitem_id, pitem_revision_id FROM PITEMREVISION
WHERE pdate_modified > &cutoff_timestamp;

-- 2.2: BOM changes: Find new or modified BOM Views and lines
-- This query identifies BOM View Revisions (BVRs) that have changed.
-- Extracting the BVR will bring over the latest BOM structure.
SELECT puid FROM PIMANRELATION
WHERE pdate_modified > &cutoff_timestamp
  AND rrelation_typeu = (SELECT puid FROM PIMANTYPE WHERE ptype_name = 'PSBOMView');

-- 2.3: Workflow changes: Find processes that are still running or were started since cutoff
-- Any process that is still running needs to be handled (e.g., terminated or restarted in the new system).
SELECT puid FROM PEPM_PROCESS
WHERE pdate_modified > &cutoff_timestamp OR pstate = 'STARTED';
