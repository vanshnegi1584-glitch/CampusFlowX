
@echo off
cd /d "%~dp0"

if not exist build mkdir build

g++ -std=c++17 -Wall -Wextra -Iinclude src\main.cpp src\Menus.cpp src\Timetable.cpp src\ClassroomFinder.cpp src\Assignment.cpp src\AssignmentManager.cpp src\RouteNavigator.cpp src\InputValidator.cpp src\DateTimeUtils.cpp -o build\CampusFlowX.exe

if errorlevel 1 (
    echo Compilation failed. Check the errors above.
    pause
    exit /b 1
)

echo Build successful!
echo Starting Campus FlowX...
build\CampusFlowX.exe
pause
