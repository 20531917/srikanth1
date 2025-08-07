@echo off
REM
REM 16. LOAD TEAMCENTER PROJECTS
REM
REM PURPOSE:
REM This utility imports the Teamcenter Project information into the target system.
REM
REM NOTE:
REM As with extraction, `plmxml_import` is the standard tool. The organization
REM (users, groups, roles) and the project data (items, etc.) must already
REM exist in the target system before running this import.
REM
REM OUTPUT:
REM - Projects created in the target Teamcenter.
REM - Log files from the import process.
REM
REM USAGE (wrapping a PLMXML import):
REM plmxml_import.exe -u=<user> -p=<password> -g=<group> -xml_file=projects.xml
REM
ECHO Executing: Placeholder for 16_load_projects
ECHO Purpose: Import Teamcenter Project information into the target system.
ECHO The recommended approach is to use 'plmxml_import'.
ECHO --------------------------------------------------------------------------------
REM
REM Pseudo-code for a script wrapping 'plmxml_import':
REM
REM 1. Ensure all users, groups, roles, and data referenced by the project
REM    have already been migrated to the target system.
REM 2. Construct the plmxml_import command line.
REM 3. Execute the command.
REM 4. Review the logs very carefully for any errors related to missing users
REM    or missing data.
REM
ECHO Output -> Log file from the PLMXML import utility.
EXIT /B 0
