@echo off
title JSON Data -> Custom Flash Storage (PlatformIO)
setlocal ENABLEDELAYEDEXPANSION

REM ============================
REM CONFIGURATION
REM V16.4.2-2026-01-11T21:50:00Z - Fixed esptool path for PlatformIO
REM ============================
set PROJECT_DIR=%~dp0
set PROJECT_TOOLS=%PROJECT_DIR%tools\FFAT
set DATA_IN=%PROJECT_DIR%data_in
set DATA_OUT=%PROJECT_DIR%data
set IMAGE_OUT=%PROJECT_DIR%ffat.bin
set MANIFEST_OUT=%PROJECT_DIR%ffat_manifest.txt

set PORT=COM3
set BAUD=921600
set FFAT_SIZE=917504

REM V16.4.2-2026-01-11T21:50:00Z - PlatformIO uses esptool.py not .exe
set ESPTOOL=python "%USERPROFILE%\.platformio\packages\tool-esptoolpy\esptool.py"

REM Python executable
set PYTHON=python

REM Check for dry-run flag
set DRYRUN=0
if "%1"=="--dry-run" set DRYRUN=1

echo ========================================
echo JSON to Custom Flash Storage
echo V16.4.2-2026-01-11T21:50:00Z - PlatformIO
echo ========================================
echo.

REM ============================
REM VERIFY PATHS EXIST
REM ============================
if not exist "%PROJECT_TOOLS%\minify_json.py" (
    echo ERROR: Cannot find minify_json.py
    echo Looking in: %PROJECT_TOOLS%
    pause
    goto :EOF
)

if not exist "%DATA_IN%" (
    echo ERROR: data_in folder not found
    mkdir "%DATA_IN%"
    echo Created: %DATA_IN%
    echo IMPORTANT: Add your JSON files to data_in\ folder!
    pause
    goto :EOF
)

REM ============================
REM CLEAN DATA OUT
REM ============================
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"
mkdir "%DATA_OUT%"

REM ============================
REM MINIFY JSON FILES
REM ============================
echo Step 1: Minifying JSON files...
%PYTHON% "%PROJECT_TOOLS%\minify_json.py" "%DATA_IN%" "%DATA_OUT%"
if errorlevel 1 (
    echo ERROR: Minification failed
    pause
    goto :EOF
)
echo Minification complete!

REM ============================
REM GENERATE MANIFEST
REM ============================
echo.
echo Step 2: Generating manifest...
%PYTHON% "%PROJECT_TOOLS%\generate_manifest.py" "%DATA_OUT%" "%MANIFEST_OUT%"
if errorlevel 1 (
    echo ERROR: Manifest generation failed
    pause
    goto :EOF
)
echo Manifest complete!

if %DRYRUN%==1 (
    echo Dry-run complete. No image created.
    pause
    goto :EOF
)

REM ============================
REM BUILD SIMPLE STORAGE IMAGE
REM ============================
echo.
echo Step 3: Building simple storage image...
%PYTHON% "%PROJECT_TOOLS%\build_simple_storage.py" "%DATA_OUT%" "%IMAGE_OUT%" %FFAT_SIZE% --manifest "%MANIFEST_OUT%"
if errorlevel 1 (
    echo ERROR: Image build failed
    pause
    goto :EOF
)

if not exist "%IMAGE_OUT%" (
    echo ERROR: Image file not created!
    pause
    goto :EOF
)

echo Image created successfully!

REM ============================
REM FLASH IMAGE TO ESP32
REM V16.4.2-2026-01-11T21:50:00Z - Call esptool.py with python
REM ============================
echo.
echo Step 4: Flashing image to ESP32...
%ESPTOOL% --chip esp32 --port %PORT% --baud %BAUD% write_flash 0x290000 "%IMAGE_OUT%"
if errorlevel 1 (
    echo ERROR: Flash failed
    pause
    goto :EOF
)
echo Flash complete!

REM ============================
REM CLEAN STAGING FOLDER
REM ============================
echo.
echo Cleaning staging folder...
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"

echo.
echo ========================================
echo SUCCESS! Flash storage updated!
echo ========================================
echo.
echo Next steps:
echo 1. Press ESP32 reset button
echo 2. Watch serial monitor for "Files in storage: N" - N should match
echo    the file count printed by this script above.
echo.
pause
endlocal
