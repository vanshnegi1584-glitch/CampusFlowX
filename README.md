# CampusFlowX

CampusFlowX is an offline, menu-driven campus assistant developed as a B.Tech Project-Based Learning (PBL) project. It helps students view their weekly timetable, find free periods, check classroom availability, manage assignments, plan study time, and navigate between campus locations. Teachers can check classroom availability and view room schedules.

The application runs in the terminal and uses local CSV files for data storage. It does not require an internet connection, web server, database server, or third-party runtime service.

## Team Members

| S. No. | Name               | Role        | Email                     | GitHub Profile       |
| -----: | ------------------ | ----------- | ------------------------- | -------------------- |
|      1 | Akhil Singh Koteri | Team Lead   | koterinikhil@gmail.com    | akhilsinghkoteri     |
|      2 | Mayank Bisht       | Team Member | mayankbisht2325@gmail.com | mayankbisht2325-gif  |
|      3 | Aditya Singh       | Team Member | pundiraditya33@gmail.com  | Aditya20-pp          |
|      4 | Vansh Negi         | Team Member | vanshnegi1584@gmail.com   | vanshnegi1584-glitch |

> Replace “To be added” with each member’s GitHub profile URL when available.

## Features

### Student Module

- Create a student session by entering a name, course, semester, and section.
- View the timetable loaded for the selected course, semester, and section.
- Find free periods from the loaded weekly timetable.
- View classroom availability and room schedules.
- Find routes to campus locations and classrooms when route data is available.
- Add, view, search, edit, and delete assignments.
- Track assignment deadlines, priority, status, estimated duration, and completed time.
- Get quick assignment recommendations using the greedy scheduling algorithm.
- Get time-constrained assignment recommendations using 0/1 knapsack dynamic programming.

### Teacher Module

- Create a teacher session by entering a name and optional department.
- Check whether a classroom is available during a requested time interval.
- View the complete schedule for a classroom.

### Campus Route Module

- Load campus locations and connections from `campus_graph.csv`.
- Find a shortest route between supported locations.
- Calculate route distance using the travel-time values stored in the graph data.

## Data Structures and Algorithms

The following data structures and algorithms are implemented in the source code:

| Data Structure / Algorithm       | Implementation                             | Purpose                                                              |
| -------------------------------- | ------------------------------------------ | -------------------------------------------------------------------- |
| Singly linked list               | `src/DsaList.c`                            | Stores assignment nodes                                              |
| Merge sort                       | `src/DsaList.c`                            | Sorts assignment linked-list nodes by deadline key                   |
| Graph using adjacency lists      | `src/RouteNavigator.cpp`, `src/DsaGraph.c` | Represents campus locations and connections                          |
| Binary min-heap                  | `src/DsaGraph.c`                           | Selects the next minimum-distance vertex during shortest-path search |
| Dijkstra’s algorithm             | `src/DsaGraph.c`                           | Finds a minimum-travel-time route                                    |
| Greedy scheduling                | `src/DsaScheduler.c`                       | Selects suitable tasks according to score and available time         |
| 0/1 Knapsack dynamic programming | `src/DsaScheduler.c`                       | Selects a high-value combination of tasks within a time limit        |

## Technologies Used

- **C (C11):** Core data-structure and algorithm implementations.
- **C++ (C++17):** Application classes, menus, file handling, input validation, and business logic.
- **CSV files:** Local storage for timetable, assignment, and campus route data.
- **MinGW-w64 GCC/G++:** Windows compiler toolchain used by `build.bat`.
- **Visual Studio Code:** Recommended editor for viewing and building the source code.

## Project Structure

```text
Campus FlowX/
├── build/
│   └── CampusFlowX.exe       # Included Windows executable
├── data/
│   ├── assignments.csv       # Assignment records
│   ├── campus_graph.csv      # Campus locations and connections
│   └── timetable.csv         # Timetable records
├── include/                  # Header files and class declarations
├── src/
│   ├── DsaList.c             # Linked list and merge sort
│   ├── DsaGraph.c            # Binary min-heap and Dijkstra
│   ├── DsaScheduler.c        # Greedy and knapsack algorithms
│   └── ...                   # Application source files
├── build.bat                 # Windows build-and-run script
├── README.md                 # Project documentation
└── .gitignore
```

## Requirements

To compile the project on Windows, install:

1. **Visual Studio Code** or another code editor.
2. **MinGW-w64 GCC/G++** with C11 and C++17 support.
3. Add the MinGW-w64 `bin` directory to the Windows `PATH` environment variable.

The `gcc` and `g++` commands must work in the VS Code terminal. The C/C++ extension for VS Code is helpful for editing and IntelliSense, but it does not install the compiler by itself.

## Build and Run on Windows Using VS Code

1. Extract the complete project ZIP file.
2. Open Visual Studio Code.
3. Select **File → Open Folder** and open the `Campus FlowX` project folder (the folder containing `src`, `include`, and `data`).
4. Open the VS Code terminal using **Terminal → New Terminal**.
5. Check that the compiler is available:

   ```powershell
   gcc --version
   g++ --version
   ```

6. Run the build script:

   ```powershell
   .\build.bat
   ```

The script compiles the C source files with GCC, compiles and links the C++ application with G++, creates `build\CampusFlowX.exe`, and launches the application if compilation succeeds.

If Windows reports that `gcc` or `g++` is not recognised, install/configure MinGW-w64 and add its `bin` folder to `PATH`, then reopen VS Code.

## Run the Existing Executable

If you do not want to rebuild, run the executable included in the ZIP from the project root:

```powershell
.\build\CampusFlowX.exe
```

Keep the `data` folder in its original location so the application can load its CSV files.

## Data Files

The application uses the following files from the `data` directory:

- `timetable.csv` — contains course, semester, section, day, subject, start time, end time, and classroom information. This file is required for the application to start.
- `assignments.csv` — stores assignment details and progress. If it is absent, the application can start with an empty assignment list and save assignment data when required.
- `campus_graph.csv` — stores campus locations and connections used by route navigation. If it is missing, route functionality may be unavailable while other modules can still run.

Keep the CSV files in the `data` folder and preserve their column names and structure when editing them.

## Important Notes and Limitations

- CampusFlowX is an offline academic project; it does not provide live classroom booking or live timetable updates.
- Classroom availability is calculated from the timetable data supplied with the project.
- Route results are limited to locations and connections present in `campus_graph.csv`.
- Assignment data is stored locally in a CSV file and is not synchronised between computers.
- Student and teacher details are entered for the current application session; this project does not provide an online account or authentication system.
- The project ZIP does not include a separate automated test suite or a CMake/Make build configuration.

## Academic Project

- **Project Name:** CampusFlowX
- **Project Type:** B.Tech Project-Based Learning (PBL)
- **Programming Languages:** C and C++
- **Standards:** C11 and C++17
- **Application Type:** Offline, menu-driven terminal application
