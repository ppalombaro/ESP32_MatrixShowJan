@echo off
title JSON Data → FFAT Build + Flash Tool
setlocal ENABLEDELAYEDEXPANSION

REM ============================
REM CONFIGURATION
REM ============================
set PROJECT_DIR=D:\patpa\Documents\Arduino\ESP32_MatrixShow
set PROJECT_TOOLS=%PROJECT_DIR%\tools\FFAT
set DATA_IN=%PROJECT_DIR%\data_in
set DATA_OUT=%PROJECT_DIR%\data
set IMAGE_OUT=%PROJECT_DIR%\ffat.bin
set MANIFEST_OUT=%PROJECT_DIR%\ffat_manifest.txt

set PORT=COM3
set BAUD=921600
set FFAT_SIZE=917504

REM Path to esptool.exe
set ESPTOOL=C:\Users\patpa\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe

REM Python executable
set PYTHON=python

REM Check for dry-run flag
set DRYRUN=0
if "%1"=="--dry-run" set DRYRUN=1

echo ========================================
echo JSON → FFAT Build Tool
echo ========================================

REM ============================
REM CLEAN DATA OUT
REM ============================
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"
mkdir "%DATA_OUT%"

REM ============================
REM MINIFY JSON FILES
REM ============================
echo Minifying JSON files...
if %DRYRUN%==1 (
    %PYTHON% "%PROJECT_TOOLS%\minify_json.py" "%DATA_IN%" "%DATA_OUT%" --dry-run
) else (
    %PYTHON% "%PROJECT_TOOLS%\minify_json.py" "%DATA_IN%" "%DATA_OUT%"
)
if errorlevel 1 (
    echo ERROR: Minification failed
    pause
    goto :EOF
)
pause

REM ============================
REM BUILD FFAT IMAGE
REM ============================
echo Building FFAT image...
if %DRYRUN%==1 (
    %PYTHON% "%PROJECT_TOOLS%\build_ffat_image.py" "%DATA_OUT%" "%IMAGE_OUT%" %FFAT_SIZE% --dry-run --manifest "%MANIFEST_OUT%"
) else (
    %PYTHON% "%PROJECT_TOOLS%\build_ffat_image.py" "%DATA_OUT%" "%IMAGE_OUT%" %FFAT_SIZE% --manifest "%MANIFEST_OUT%"
)
if errorlevel 1 (
    echo ERROR: FFAT image build failed
    pause
    goto :EOF
)
pause

if %DRYRUN%==1 (
    echo Dry-run complete. No image flashed.
    pause
    goto :EOF
)

REM ============================
REM FLASH IMAGE TO ESP32
REM ============================
echo Flashing FFAT image...
"%ESPTOOL%" --chip esp32 --port %PORT% --baud %BAUD% write_flash 0x290000 "%IMAGE_OUT%"
if errorlevel 1 (
    echo ERROR: Flash failed
    pause
    goto :EOF
)
pause

REM ============================
REM POST-FLASH VERIFY
REM ============================
echo Verifying flashed FFAT image SHA256...
python "%PROJECT_TOOLS%\verify_ffat_flash.py" --exe "%ESPTOOL%" --port %PORT% --offset 0x290000 --size %FFAT_SIZE% --manifest "%MANIFEST_OUT%"
if errorlevel 1 (
    echo ERROR: Flash verification failed
    pause
    goto :EOF
)
pause

REM ============================
REM CLEAN STAGING FOLDER
REM ============================
echo Cleaning staging folder...
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"

echo FFAT build + flash complete!
pause
endlocal
