#include "Menus.h"
#include "InputValidator.h"
#include <iostream>
#include <exception>

using namespace std;

int main() {
    try {
        // Run from the project folder so these three paths stay easy to follow.
        Timetable timetable;
        timetable.loadFromCSV("data/timetable.csv");
        ClassroomFinder finder(timetable);
        AssignmentManager assignments;
        assignments.loadFromCSV("data/assignments.csv");
        RouteNavigator navigator;
        navigator.loadGraph("data/campus_graph.csv");

        while (true) {
            cout << "\nCAMPUS FLOWX - SIMPLE\n1. Timetable\n2. Classroom Finder\n"
                         "3. Assignment Manager\n4. Route Planner\n5. Exit\n";
            int choice = InputValidator::getIntInput("Choice: ", 1, 5);
            if (choice == 5) {
                break;
            }
            if (choice == 1) {
                timetableMenu(timetable);
            }
            else if (choice == 2) {
                classroomMenu(finder);
            }
            else if (choice == 3) {
                assignmentMenu(assignments);
            }
            else {
                routeMenu(navigator);
            }
        }
    } catch (const InputValidator::EndOfInput&) {
        cout << "\nInput closed. An unfinished assignment was not added.\n";
    } catch (const exception& error) {
        cerr << "Error: " << error.what()
                  << "\nRun from CampusFlowX_Simple and check its data files.\n";
        return 1;
    }
    cout << "Goodbye! Completed changes have already been saved.\n";
    return 0;
}
