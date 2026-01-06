@echo off
REM ===========================================
REM List all files in the FFAT/data folder with sizes
REM Usage: double-click or run from command line
REM ===========================================

REM Set your data folder path here:
SET "DATA_FOLDER=.\data"

REM Output file for file sizes
SET "OUTPUT_FILE=file_sizes.txt"

echo Listing all files in "%DATA_FOLDER%" ...
echo File sizes in bytes > "%OUTPUT_FILE%"
echo ------------------------ >> "%OUTPUT_FILE%"

REM Recursively list all files with sizes
for /R "%DATA_FOLDER%" %%F in (*) do (
    set FILESIZE=%%~zF
    set FILENAME=%%F
    call echo %%FILESIZE%%    %%F >> "%OUTPUT_FILE%"
)

echo Done! File sizes saved in "%OUTPUT_FILE%"
pause
