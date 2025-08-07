@echo off
REM
REM 13. EXTRACT ORGANIZATION
REM
REM PURPOSE:
REM This utility extracts organization data: users, groups, roles, and the
REM hierarchy of groups.
REM
REM NOTE:
REM Organization data is often best moved using the `plmxml_export` utility,
REM as it correctly handles the complex relationships between users, groups, and roles.
REM A separate export for each type of object is a safe approach.
REM
REM ITK FUNCTIONS TO USE (if not using PLMXML):
REM - SA_list_groups() to get all groups.
REM - SA_ask_group_members() to get users in a group.
REM - SA_find_user() to get user details.
REM - SA_find_role() to get role details.
REM
REM OUTPUT:
REM - PLMXML files for organization components.
REM - Or, custom CSV/XML files detailing the organization structure.
REM
REM USAGE (wrapping a PLMX_export):
REM plmxml_export.exe -u=infodba -p=... -g=... -class=Group -xml_file=groups.xml
REM plmxml_export.exe -u=infodba -p=... -g=... -class=Role -xml_file=roles.xml
REM plmxml_export.exe -u=infodba -p=... -g=... -class=User -xml_file=users.xml
REM
ECHO Executing: Placeholder for 13_extract_organization
ECHO Purpose: Extract users, groups, and roles from the source Teamcenter.
ECHO The recommended approach is to use 'plmxml_export' for each class.
ECHO --------------------------------------------------------------------------------
REM
REM Pseudo-code for a script wrapping 'plmxml_export':
REM
REM 1. Export all Groups to a groups.xml file.
REM 2. Export all Roles to a roles.xml file.
REM 3. Export all Users to a users.xml file.
REM 4. Check logs for any errors.
REM
ECHO Output -> PLMXML files for groups, roles, and users.
EXIT /B 0
