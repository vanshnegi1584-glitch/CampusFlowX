# Campus FlowX — Smart Campus Assistant

## 1. Project Overview

Campus FlowX is a simple, terminal-based campus assistant developed using C++17. It helps students find timetable information, check classroom availability, manage assignments, and navigate between campus locations.

This is an initial implementation of the project that focuses on core features and beginner-friendly code.

## 2. Features

### Module 1: Timetable and Classroom Finder

- Display the class timetable.
- Search for subjects and classes.
- View assigned classrooms.
- Check classroom availability.

### Module 2: Assignment Manager

- Add new assignments.
- View all assignments.
- Mark assignments as completed.
- Store assignment information locally.

### Module 3: Route Planner and Navigation

- Represent campus locations using a graph.
- Find routes between locations using Breadth-First Search (BFS).
- Display the path between the selected locations.

### Module 4: Main Menu and Integration

- Access all modules through a single menu.
- Select a feature using the keyboard.
- Exit the application.

## 3. Technologies Used

- Programming Language: C++17
- Compiler: MinGW g++
- IDE: Visual Studio Code
- Data Storage: CSV and text files
- Algorithm: Breadth-First Search (BFS)

## 4. Project Structure

```text
CampusFlowX_Simple/
├── include/       # Header files
├── src/            # C++ implementation files
├── data/           # Sample datasets
├── build/          # Compiled executable
├── build.bat       # Build and run script
└── README.md       # Project documentation
```

## 5. Requirements

Before running the project, make sure you have:

- Windows 10 or Windows 11
- Visual Studio Code (optional but recommended)
- MinGW g++ compiler with C++17 support
- A terminal or command prompt

## 6. How to Build and Run

### Method 1: Using build.bat

1. Extract the project ZIP file.
2. Open the `CampusFlowX_Simple` folder.
3. Ensure that MinGW g++ is installed and added to your system PATH.
4. Double-click `build.bat`.
5. The script will compile the source files and generate the executable.
6. The program will start automatically if compilation succeeds.

### Method 2: Using the Terminal

Open a terminal in the project folder and run:

```bat
build.bat
```

The executable will be generated at:

```text
build/CampusFlowX.exe
```

## 7. How to Use

1. Run the program.
2. Select an option from the main menu.
3. Follow the on-screen instructions.
4. Enter the required information.
5. Return to the main menu to use another module.
6. Select Exit when finished.

## 8. Data Files

The `data` folder contains sample data used by the application, such as timetable information, assignments, and campus locations.

Do not remove required data files, as some features may depend on them.

## 9. Current Project Scope

This version focuses on the basic working features of Campus FlowX. Advanced features, graphical interfaces, authentication, and online services are outside the scope of this initial implementation.

The project can be extended in future versions with additional functionality, improved data management, and more campus navigation features.

## 10. Team Responsibilities

- Member 1: Timetable and Classroom Finder
- Member 2: Assignment Manager
- Member 3: Route Planner and Navigation
- Member 4: Main Program and Integration

## 11. Conclusion

Campus FlowX demonstrates how basic C++ programming, classes, file handling, and graph algorithms can be combined to create a useful campus assistant. The project provides a foundation for further development and improvement.
