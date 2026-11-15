#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "Student.h"
#include "Teacher.h"
#include "Menus.h"
#include "Timetable.h"
#include "ClassroomFinder.h"
#include "AssignmentManager.h"
#include "RouteNavigator.h"
#include "DateTimeUtils.h"
#include "InputValidator.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = filesystem;
using namespace InputValidator;

namespace {
void displayMainMenu() {
    cout << "\n=============================================\n";
    cout << " CAMPUS FLOWX - CLASSROOM & PRODUCTIVITY SYSTEM\n";
    cout << "=============================================\n";
    cout << "1. Student\n";
    cout << "2. Teacher\n";
    cout << "0. Exit\n";
}

} // namespace

int main(int argc, char** argv) {
    fs::path dataDirectory;
    string assignmentFile;
    AssignmentManager assignments;
    bool assignmentsLoaded = false;
    try {
        fs::path executable = fs::absolute(argv[0]);
#ifdef _WIN32
        std::vector<wchar_t> executablePath(32768);
        DWORD length = GetModuleFileNameW(nullptr, executablePath.data(),
                                          static_cast<DWORD>(executablePath.size()));
        if (length > 0 && length < executablePath.size())
            executable = fs::path(std::wstring(executablePath.data(), length));
#endif
        if (argc == 2 && string(argv[1]) == "--help") {
            cout
                << "Campus FlowX - offline campus assistant\n"
                << "Usage: CampusFlowX [--data-dir DIRECTORY]\n"
                << "The directory contains timetable.csv, assignments.csv, and campus_graph.csv.\n";
            return 0;
        }
        if (argc == 3 && string(argv[1]) == "--data-dir") {
            dataDirectory = fs::absolute(argv[2]);
        } else if (argc != 1) {
            cerr << "Usage: CampusFlowX [--data-dir DIRECTORY]\n";
            return 1;
        } else {
            for (const auto& candidate :
                 vector<fs::path>{executable.parent_path() / "data",
                                  executable.parent_path().parent_path() / "data",
                                  fs::current_path() / "data"}) {
                if (fs::is_regular_file(candidate / "timetable.csv")) {
                    dataDirectory = candidate;
                    break;
                }
            }
            if (dataDirectory.empty())
                throw runtime_error("Timetable data not found. Use --data-dir DIRECTORY.");
        }
        cout << "Campus FlowX | " << DateTimeUtils::getCurrentDate() << '\n';
        Timetable timetable;
        if (!timetable.loadFromCSV((dataDirectory / "timetable.csv").string()))
            return 1;
        ClassroomFinder finder(timetable);
        assignmentFile = (dataDirectory / "assignments.csv").string();
        if (fs::exists(assignmentFile) && !assignments.loadFromCSV(assignmentFile)) {
            cerr << "Assignments could not be loaded safely. Correct the reported file errors and "
                    "restart.\n";
            return 1;
        }
        assignmentsLoaded = true;
        RouteNavigator navigator((dataDirectory / "campus_graph.csv").string());
        while (true) {
            displayMainMenu();
            int choice = getMenuChoice(0, 2);
            if (choice == 0)
                break;
            if (choice == 1) {
                string name = getNonEmptyStringInput("Enter your name: ");
                cout << "Available courses:\n";
                for (const auto& course : timetable.getAllCourses())
                    cout << "  " << course << '\n';
                string course = getValidCourseInput("Enter course: ", timetable.getAllCourses());
                const auto semesters = timetable.getSemesters(course);
                cout << "Available semesters:";
                for (int semester : semesters)
                    cout << ' ' << semester;
                cout << '\n';
                int semester;
                while (true) {
                    semester = getValidSemesterInput("Enter semester: ", 1, 12);
                    if (find(semesters.begin(), semesters.end(), semester) != semesters.end())
                        break;
                    cout << "No timetable is loaded for that semester. Choose one listed above.\n";
                }
                const auto sections = timetable.getSections(course, semester);
                cout << "Available sections:";
                for (const auto& section : sections)
                    cout << ' ' << section;
                cout << '\n';
                string section;
                while (true) {
                    section = getSectionInput("Enter section: ");
                    if (find(sections.begin(), sections.end(), "ALL") != sections.end() ||
                        find(sections.begin(), sections.end(), section) != sections.end())
                        break;
                    cout << "Choose one of the sections listed above.\n";
                }
                Student student(name, course, semester, section);
                studentMenu(student, timetable, finder, assignments, navigator, assignmentFile);
            } else {
                string name = getNonEmptyStringInput("Enter your name: ");
                string dept = getStringInput("Enter department (optional): ", true);
                Teacher teacher(name, dept);
                teacher.displayProfile();
                teacherMenu(finder);
            }
        }
    } catch (const EndOfInput&) {
        cout << "\nInput closed. Ending the session.\n";
    } catch (const std::exception& e) {
        cerr << "\nError: " << e.what() << '\n';
        if (assignmentsLoaded)
            assignments.saveToCSV(assignmentFile);
        return 1;
    }
    if (assignmentsLoaded && !assignments.saveToCSV(assignmentFile))
        return 1;
    cout << "Assignments saved. Thank you for using Campus FlowX!\n";
    return 0;
}
