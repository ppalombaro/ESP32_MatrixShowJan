@echo off
setlocal

echo ========================================
echo ESP32 FFAT Minimize + Upload Tool
echo ========================================
echo.

REM ---------- CONFIG ----------
set DATA_FOLDER=D:\patpa\Documents\Arduino\ESP32_MatrixShow\data
set DATA_MIN=D:\patpa\Documents\Arduino\ESP32_MatrixShow\data_min
set FFAT_MAX=917504

REM Path to ESP32FS JAR (Java must be installed and in PATH)
set ESP32FS=java -jar C:\Users\patpa\AppData\Local\Arduino15\tools\ESP32FS\tool\esp32fs.jar

REM COM port of ESP32
set PORT=COM3
REM ---------------------------

echo Starting FFAT minimize + upload process...
pause

REM ---------- CLEAN MINIMIZED FOLDER ----------
echo.
echo Cleaning minimized data folder...
if exist "%DATA_MIN%" rmdir /s /q "%DATA_MIN%"
mkdir "%DATA_MIN%"

REM ---------- MINIMIZE JSON ----------
echo.
echo Minimizing JSON files...
python "%~dp0minimize_ffat.py"
set PYERR=%ERRORLEVEL%

if %PYERR% NEQ 0 (
    echo [XX] ABORTING: Minimized data exceeds FFAT limit or Python error
    goto END
)

echo [OK] Minimization completed successfully.

REM ---------- CONFIRM UPLOAD ----------
echo.
set /p CONFIRM="Proceed with FFAT upload? (Y/N): "
if /I NOT "%CONFIRM%"=="Y" (
    echo Upload canceled by user.
    goto END
)

REM ---------- UPLOAD FFAT ----------
echo.
echo Uploading FFAT data via ESP32FS JAR...
call "%ESP32FS%" --port %PORT% upload "%DATA_MIN%"
if errorlevel 1 (
    echo [XX] FFAT upload failed!
    goto END
)

echo [OK] FFAT upload completed successfully.

:END
echo.
echo ========================================
echo Process finished.
echo Press ANY KEY to close this window.
echo ========================================
pause >nul
