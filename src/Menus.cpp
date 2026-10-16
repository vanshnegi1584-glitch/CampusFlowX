#include "Menus.h"
#include "InputValidator.h"
#include <climits>
#include <iostream>
#include <exception>

using namespace std;

void timetableMenu(const Timetable& timetable) {
    while (true) {
        cout << "\nTimetable\n1. Display timetable\n2. Search subject/class\n3. Back\n";
        int choice = InputValidator::getIntInput("Choice: ", 1, 3);
        if (choice == 3) {
            return;
        }
        if (choice == 1) {
            timetable.displayAll();
        }
        else {
            timetable.searchSubject(InputValidator::getStringInput("Subject keyword: "));
        }
    }
}

void classroomMenu(const ClassroomFinder& finder) {
    finder.displayRooms();
    string room = InputValidator::getStringInput("Room: ");
    string day = InputValidator::getDayInput();
    int time = InputValidator::getTimeInput();
    finder.checkRoomAvailability(room, day, time);
}

void assignmentMenu(AssignmentManager& assignments) {
    while (true) {
        cout << "\nAssignment Manager\n1. Add assignment\n2. Display assignments\n"
                     "3. Mark completed\n4. Back\n";
        int choice = InputValidator::getIntInput("Choice: ", 1, 4);
        if (choice == 4) {
            return;
        }
        try {
            if (choice == 1) {
                string title = InputValidator::getStringInput("Title: ");
                string subject = InputValidator::getStringInput("Subject: ");
                string dueDate = InputValidator::getDateInput();
                assignments.addAssignment(title, subject, dueDate);
                cout << "Assignment added and saved.\n";
            } else if (choice == 2) {
                assignments.displayAll();
            } else {
                int id = InputValidator::getIntInput("Assignment ID: ", 1, INT_MAX - 1);
                if (assignments.markCompleted(id)) {
                    cout << "Assignment completed and saved.\n";
                }
                else {
                    cout << "Assignment ID not found.\n";
                }
            }
        } catch (const exception& error) {
            cerr << "Assignment action failed: " << error.what() << '\n';
        }
    }
}

void routeMenu(const RouteNavigator& navigator) {
    navigator.displayLocations();
    string source = InputValidator::getStringInput("Source: ");
    string destination = InputValidator::getStringInput("Destination: ");
    try {
        vector<string> route = navigator.findShortestRoute(source, destination);
        if (route.empty()) {
            cout << "No route found between these locations in the sample graph.\n";
            return;
        }
        cout << "BFS route: ";
        for (size_t index = 0; index < route.size(); ++index) {
            if (index > 0) {
                cout << " -> ";
            }
            cout << route[index];
        }
        cout << "\nConnections: " << route.size() - 1
                  << " (fewest edges; not travel time).\n";
    } catch (const exception& error) {
        cout << error.what() << '\n';
    }
}
