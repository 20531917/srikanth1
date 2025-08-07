# ITK Source Code for Teamcenter Migration Utilities

This directory contains the C++ source code for the command-line utilities used in the data migration process. These utilities are designed to be compiled and run in a Teamcenter environment.

## 1. Prerequisites

To compile and run these utilities, you will need:
- A Windows development machine.
- Microsoft Visual Studio (e.g., 2015 or later).
- A Teamcenter 2-tier or 4-tier client installation. This provides the necessary ITK libraries and header files.
- The `TC_ROOT` environment variable must be set to the Teamcenter installation directory (e.g., `C:\Siemens\Teamcenter12`).

## 2. Compiling the Code (Windows with Visual Studio)

The following instructions describe how to compile the code using a Visual Studio command prompt.

### 2.1. Open the Developer Command Prompt

Open the "Developer Command Prompt for VS" (or a similar shortcut) from your Start Menu. This sets up the environment with the necessary compiler and linker paths.

### 2.2. Set Up the Environment

You need to run the `tc_profilevars.bat` script to set up the Teamcenter-specific environment variables.

```batch
call %TC_ROOT%\bin\tc_profilevars.bat
```

### 2.3. Compile a Utility

To compile one of the utilities (e.g., `01_extract_items_and_revisions.cpp`), you use the `cl.exe` compiler. You must include the ITK header directories and link against the ITK libraries.

The `-I` flag points to include directories, and the `-link` flag specifies libraries to link against.

```batch
cl.exe 01_extract_items_and_revisions.cpp -I%TC_ROOT%\include -I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libpom.lib libps.lib libae.lib libgrm.lib libsa.lib libtccore.lib
```

This will create an executable file named `01_extract_items_and_revisions.exe` in the current directory.

### 2.4. Compiling All Utilities

You can create a simple batch file (`compile_all.bat`) in this directory to compile all utilities at once:

```batch
@echo off
call %TC_ROOT%\bin\tc_profilevars.bat

echo Compiling...
cl 01_extract_items_and_revisions.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libqry.lib
cl 02_extract_bom_structures.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libps.lib
cl 03_extract_datasets.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libgrm.lib libae.lib
cl 04_export_physical_files.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libae.lib libsa.lib
cl 06_import_items_and_revisions.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib
cl 07_import_datasets.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libae.lib libgrm.lib
cl 08_import_physical_files.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libae.lib libsa.lib
cl 09_import_bom_structures.cpp /EHsc /I%TC_ROOT%\include /I%TC_ROOT%\include_cpp -link /LIBPATH:%TC_ROOT%\lib libitk.lib libtccore.lib libpom.lib libps.lib

echo Compilation finished.
```

## 3. Usage

Once compiled, the `.exe` files can be called from the `.bat` scripts in the `etl/` directory. You will need to modify the `.bat` scripts to call these executables instead of being placeholders.
