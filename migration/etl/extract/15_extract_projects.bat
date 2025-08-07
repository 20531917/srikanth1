@echo off
REM
REM 15. EXTRACT TEAMCENTER PROJECTS
REM
REM PURPOSE:
REM This utility extracts Teamcenter Project information, including the project
REM team (users and roles) and any data associated with the project.
REM
REM NOTE:
REM PLMXML is the preferred tool for migrating Teamcenter Projects.
REM
REM ITK FUNCTIONS TO USE (if not using PLMXML):
REM - PROJ_list_projects() to find all projects.
REM - PROJ_ask_project_info() for project metadata.
REM - PROJ_ask_team() to get the list of team members.
REM - PROJ_ask_data() to find the data folders within the project.
REM
REM OUTPUT:
REM - A PLMXML file containing the project definitions.
REM
REM USAGE (wrapping a PLMXML export):
REM plmxml_export.exe -u=<user> -p=<password> -g=<group> -class=TC_Project -xml_file=projects.xml
REM
ECHO Executing: Placeholder for 15_extract_projects
ECHO Purpose: Extract Teamcenter Project information from the source system.
ECHO The recommended approach is to use 'plmxml_export'.
ECHO --------------------------------------------------------------------------------
REM
REM Pseudo-code for a script wrapping 'plmxml_export':
REM
REM 1. Construct the plmxml_export command for the TC_Project class.
REM 2. Execute the command.
REM 3. Check the log for any errors. The export will include project team
REM    information and references to data within the project.
REM
ECHO Output -> A PLMXML file with project definitions.
EXIT /B 0
