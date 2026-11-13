@echo off
setlocal

REM Always run from the folder containing this batch file.
cd /d "%~dp0"

REM Check that MinGW-w64 GCC and G++ are available in PATH.
where gcc >nul 2>nul
if errorlevel 1 (
    echo ERROR: gcc was not found.
    echo Install MinGW-w64 and add its bin folder to the Windows PATH.
    goto :failed
)

where g++ >nul 2>nul
if errorlevel 1 (
    echo ERROR: g++ was not found.
    echo Install MinGW-w64 and add its bin folder to the Windows PATH.
    goto :failed
)

if not exist "src" (
    echo ERROR: The src folder was not found.
    echo Place build.bat in the Campus FlowX project root.
    goto :failed
)

if not exist "include" (
    echo ERROR: The include folder was not found.
    goto :failed
)

if not exist "build" mkdir "build"

 echo.
echo ============================================
echo           Building CampusFlowX
echo ============================================
echo.

echo [1/2] Compiling C source files...
for %%F in (src\*.c) do (
    echo   %%F
    gcc -std=c11 -O2 -Wall -Wextra -Iinclude -c "%%F" -o "build\%%~nF.o"
    if errorlevel 1 goto :failed
)

echo.
echo [2/2] Compiling and linking C++ source files...
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude src\*.cpp build\*.o -o "build\CampusFlowX.exe"
if errorlevel 1 goto :failed

echo.
echo ============================================
echo BUILD SUCCESSFUL
echo Executable: build\CampusFlowX.exe
echo ============================================
echo.
echo Starting CampusFlowX...
echo.
"build\CampusFlowX.exe"
goto :end

:failed
echo.
echo BUILD FAILED. Read the error messages above.
echo Make sure all project folders and source files are present.

:end
echo.
pause
endlocal
