#include "Menus.h"
#include "Student.h"
#include "Timetable.h"
#include "ClassroomFinder.h"
#include "AssignmentManager.h"
#include "Scheduler.h"
#include "RouteNavigator.h"
#include "DateTimeUtils.h"
#include "InputValidator.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace InputValidator;

namespace {
string selectDate(const string& today) {
    string date = getDateInput("Enter date (DD-MM-YYYY) or press Enter for today: ", true);
    return date.empty() ? today : date;
}

string selectDestination(const string& venue) {
    auto rooms = DateTimeUtils::splitRooms(venue);
    if (rooms.size() <= 1)
        return venue;
    cout << "This class lists multiple rooms. Select the room for your group:\n";
    for (size_t i = 0; i < rooms.size(); ++i)
        cout << i + 1 << ". " << rooms[i] << '\n';
    return rooms[getMenuChoice(1, static_cast<int>(rooms.size()), "Room: ") - 1];
}

void displayRouteResult(const RouteResult& result) {
    if (!result.found) {
        cout << "\nRoute unavailable: " << result.errorMessage << "\n";
        return;
    }

    cout << "\nShortest Route:\n\n";
    for (size_t i = 0; i < result.path.size(); ++i) {
        if (i > 0)
            cout << " -> ";
        cout << result.path[i];
    }
    cout << "\n\nEstimated Travel Time: " << result.totalMinutes << " minutes\n";
}

void displayStudentMenu() {
    cout << "\n+----------------------------------------------------------+\n";
    cout << "|                    STUDENT MENU                          |\n";
    cout << "+----------------------------------------------------------+\n";
    cout << "| 1. View today's timetable                                |\n";
    cout << "| 2. View timetable by date                                |\n";
    cout << "| 3. Detect my free periods                                |\n";
    cout << "| 4. Find vacant classrooms                                |\n";
    cout << "| 5. Check when a room is free                             |\n";
    cout << "| 6. View complete room schedule                           |\n";
    cout << "| 7. Campus Route Navigator                                |\n";
    cout << "| 8. Assignment and to-do manager                          |\n";
    cout << "| 9. Get assignment recommendations for a free period      |\n";
    cout << "| 10. View my profile                                      |\n";
    cout << "| 0. Save and return to main menu                          |\n";
    cout << "+----------------------------------------------------------+\n";
}

void displayTeacherMenu() {
    cout << "\n+----------------------------------------------------------+\n";
    cout << "|                    TEACHER MENU                          |\n";
    cout << "+----------------------------------------------------------+\n";
    cout << "| 1. Find vacant classrooms                                |\n";
    cout << "| 2. Check when a room is free                             |\n";
    cout << "| 3. View complete room schedule                           |\n";
    cout << "| 0. Return to main menu                                   |\n";
    cout << "+----------------------------------------------------------+\n";
}

vector<pair<int, int>> findFreePeriods(const vector<ScheduleEntry>& entries) {
    vector<pair<int, int>> freePeriods;
    int lastEnd = DateTimeUtils::COLLEGE_START_MINUTES;

    for (const auto& entry : entries) {
        if (entry.getStartTime() > lastEnd) {
            freePeriods.push_back({lastEnd, entry.getStartTime()});
        }
        lastEnd = max(lastEnd, entry.getEndTime());
    }
    if (lastEnd < DateTimeUtils::COLLEGE_END_MINUTES) {
        freePeriods.push_back({lastEnd, DateTimeUtils::COLLEGE_END_MINUTES});
    }
    return freePeriods;
}

void classroomAction(int choice, ClassroomFinder& finder, const string& today) {
    switch (choice) {
    case 1: {
        string date = selectDate(today);
        string weekday = DateTimeUtils::getWeekdayFromDate(date);

        string startTime = getTimeInput("Enter start time (e.g., 8, 8:30, 12, 1:30, 6): ");
        string endTime = getTimeInput("Enter end time (e.g., 8, 8:30, 12, 1:30, 6): ");

        int startMin = DateTimeUtils::parseTimeInput(startTime);
        int endMin = DateTimeUtils::parseTimeInput(endTime);

        if (startMin == -1 || endMin == -1) {
            cout << "Invalid time format.\n";
            break;
        }

        if (startMin >= endMin) {
            cout << "End time must be after start time.\n";
            break;
        }

        auto rooms = finder.findVacantRooms(weekday, startMin, endMin);
        finder.displayVacantRooms(rooms, weekday, startMin, endMin);
        break;
    }
    case 2: {
        string room = getNonEmptyStringInput("Enter room number: ");
        string date = selectDate(today);
        string weekday = DateTimeUtils::getWeekdayFromDate(date);
        finder.checkRoomAvailability(room, weekday);
        break;
    }
    case 3: {
        string room = getNonEmptyStringInput("Enter room number: ");
        string date = selectDate(today);
        string weekday = DateTimeUtils::getWeekdayFromDate(date);
        finder.displayRoomSchedule(room, weekday);
        break;
    }
    }
}

void routeMenu(const Student& student, const Timetable& timetable, RouteNavigator& navigator) {
    while (true) {
        cout << "\n+----------------------------------------------------------+\n";
        cout << "|               CAMPUS ROUTE NAVIGATOR                     |\n";
        cout << "+----------------------------------------------------------+\n";
        cout << "| 1. Find shortest route                                   |\n";
        cout << "| 2. Navigate to today's classroom                         |\n";
        cout << "| 3. Navigate to current/next class                        |\n";
        cout << "| 4. Display all available locations                       |\n";
        cout << "| 0. Return to Student Menu                                |\n";
        cout << "+----------------------------------------------------------+\n";

        int navChoice = getMenuChoice(0, 4);
        if (navChoice == 0)
            break;
        string today = DateTimeUtils::getCurrentDate();
        string todayWeekday = DateTimeUtils::getWeekdayFromDate(today);

        switch (navChoice) {
        case 1: {
            string source = getNonEmptyStringInput("Enter starting location: ");
            string destination = getNonEmptyStringInput("Enter destination: ");

            RouteResult result = navigator.findShortestRoute(source, destination);
            displayRouteResult(result);
            break;
        }
        case 2: {
            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), todayWeekday);
            if (entries.empty()) {
                cout << "No classes scheduled for today.\n";
                break;
            }
            timetable.displayEntries(entries,
                                     "Today's Timetable (" + today + " - " + todayWeekday + ")");

            cout << "\nSelect an option:\n";
            cout << "1. Find route to a classroom from this timetable\n";
            cout << "2. Return to previous menu\n";
            int opt = getMenuChoice(1, 2);
            if (opt == 2)
                break;

            int entryChoice =
                getMenuChoice(1, static_cast<int>(entries.size()), "Select timetable entry: ");
            const auto& selectedEntry = entries[entryChoice - 1];
            string destination = selectedEntry.getRoomNumber();
            string normalizedDest = selectDestination(destination);

            cout << "Destination: " << normalizedDest << "\n";
            string source = getNonEmptyStringInput("Enter your current location: ");

            RouteResult result = navigator.findShortestRoute(source, normalizedDest);
            displayRouteResult(result);
            break;
        }
        case 3: {
            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), todayWeekday);
            if (entries.empty()) {
                cout << "No classes scheduled for today.\n";
                break;
            }

            int currentMinutes = DateTimeUtils::getCurrentTimeMinutes();
            const ScheduleEntry* currentClass = nullptr;
            const ScheduleEntry* nextClass = nullptr;

            for (const auto& entry : entries) {
                if (currentMinutes >= entry.getStartTime() && currentMinutes < entry.getEndTime()) {
                    currentClass = &entry;
                    break;
                }
                if (currentMinutes < entry.getStartTime()) {
                    nextClass = &entry;
                    break;
                }
            }

            const ScheduleEntry* targetClass = currentClass ? currentClass : nextClass;
            bool isCurrent = (currentClass != nullptr);

            if (!targetClass) {
                cout << "No more classes scheduled for today.\n";
                break;
            }

            string destination = targetClass->getRoomNumber();
            string normalizedDest = selectDestination(destination);

            cout << "\n"
                 << (isCurrent ? "Current Class" : "Next Class") << ": "
                 << targetClass->getSubject() << "\n";
            cout << "Room: " << normalizedDest << "\n";
            cout << "Class Starts At: " << DateTimeUtils::minutesToTime(targetClass->getStartTime())
                 << "\n";
            cout << "Current Time: " << DateTimeUtils::minutesToTime(currentMinutes) << "\n";

            int timeRemaining = targetClass->getStartTime() - currentMinutes;
            if (isCurrent) {
                timeRemaining = targetClass->getEndTime() - currentMinutes;
                cout << "Time Remaining in Class: " << timeRemaining << " minutes\n";
            } else {
                cout << "Time Remaining: " << timeRemaining << " minutes\n";
            }

            string source = getNonEmptyStringInput("Enter your current location: ");

            RouteResult result = navigator.findShortestRoute(source, normalizedDest);
            if (result.found) {
                cout << "Estimated Travel Time: " << result.totalMinutes << " minutes\n";

                if (isCurrent) {
                    cout << (result.totalMinutes < timeRemaining
                                 ? "Status: The class has started; you can arrive before it ends.\n"
                                 : "Status: This class will end before you arrive.\n");
                } else {
                    if (result.totalMinutes <= timeRemaining) {
                        cout << "Status: You should be able to reach the class on time.\n";
                    } else {
                        cout << "Status: You may arrive late. Leave immediately.\n";
                    }
                }
            }
            displayRouteResult(result);
            break;
        }
        case 4:
            navigator.displayLocations();
            break;
        }
    }
}

void assignmentMenu(AssignmentManager& assignments, const string& assignmentFile) {
    while (true) {
        cout << "\n+----------------------------------------------------------+\n";
        cout << "|           ASSIGNMENT MANAGER                             |\n";
        cout << "+----------------------------------------------------------+\n";
        cout << "| 1. Add assignment                                        |\n";
        cout << "| 2. View all assignments                                  |\n";
        cout << "| 3. View pending assignments                              |\n";
        cout << "| 4. Edit assignment                                       |\n";
        cout << "| 5. Delete assignment                                     |\n";
        cout << "| 6. Mark assignment complete                              |\n";
        cout << "| 7. Search assignment by ID                               |\n";
        cout << "| 8. Sort by deadline                                      |\n";
        cout << "| 0. Back                                                  |\n";
        cout << "+----------------------------------------------------------+\n";

        int aChoice = getMenuChoice(0, 8);
        if (aChoice == 0)
            break;

        bool changed = false;
        switch (aChoice) {
        case 1: {
            string title = getNonEmptyStringInput("Title: ");
            string subject = getNonEmptyStringInput("Subject: ");
            string deadline;
            while (true) {
                deadline = getStringInput("Deadline (DD-MM-YYYY HH:MM AM/PM): ");
                if (DateTimeUtils::isValidDeadline(deadline))
                    break;
                cout << "Invalid deadline format.\n";
            }
            int duration = getIntInput("Estimated duration (minutes): ", 1, 10000);
            char pChoice = getCharInput("Priority (H/M/L): ", "HML");
            string priority = (pChoice == 'H') ? "High" : (pChoice == 'M') ? "Medium" : "Low";

            Assignment a(assignments.getNextId(), title, subject, deadline, duration, priority,
                         "Pending", 0);
            assignments.addAssignment(a);
            changed = true;
            // IDs advance inside AssignmentManager.
            cout << "Assignment added with ID: " << a.getId() << "\n";
            break;
        }
        case 2:
            assignments.displayAll();
            break;
        case 3:
            assignments.displayPending();
            break;
        case 4: {
            int id = getIntInput("Enter assignment ID: ", 1);
            changed = assignments.editAssignment(id);
            if (!changed) {
                cout << "Assignment not found.\n";
            }
            break;
        }
        case 5: {
            int id = getIntInput("Enter assignment ID: ", 1);
            if (getYesNoInput("Are you sure you want to delete this assignment?")) {
                if (assignments.deleteAssignment(id)) {
                    changed = true;
                    cout << "Assignment deleted.\n";
                } else {
                    cout << "Assignment not found.\n";
                }
            }
            break;
        }
        case 6: {
            int id = getIntInput("Enter assignment ID: ", 1);
            Assignment* a = assignments.findAssignment(id);
            if (a) {
                a->setStatus("Completed");
                a->setCompletedMinutes(a->getEstimatedDuration());
                changed = true;
                cout << "Assignment marked as completed.\n";
            } else {
                cout << "Assignment not found.\n";
            }
            break;
        }
        case 7: {
            int id = getIntInput("Enter assignment ID: ", 1);
            Assignment* a = assignments.findAssignment(id);
            if (a) {
                a->display(1);
            } else {
                cout << "Assignment not found.\n";
            }
            break;
        }
        case 8:
            assignments.sortByDeadline();
            changed = true;
            cout << "Assignments sorted by deadline.\n";
            assignments.displayAll();
            break;
        }
        if (changed && !assignments.saveToCSV(assignmentFile)) {
            cout << "Changes remain in this session. Fix the save location before exiting.\n";
        }
    }
}
} // namespace

void studentMenu(Student& student, Timetable& timetable, ClassroomFinder& finder,
                 AssignmentManager& assignments, RouteNavigator& navigator,
                 const string& assignmentFile) {
    student.displayProfile();
    while (true) {
        displayStudentMenu();
        int choice = getMenuChoice(0, 10);

        string today = DateTimeUtils::getCurrentDate();
        string todayWeekday = DateTimeUtils::getWeekdayFromDate(today);

        switch (choice) {
        case 1: {
            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), todayWeekday);
            timetable.displayEntries(entries,
                                     "Today's Timetable (" + today + " - " + todayWeekday + ")");
            break;
        }
        case 2: {
            string date = getDateInput("Enter date (DD-MM-YYYY): ");
            string weekday = DateTimeUtils::getWeekdayFromDate(date);
            if (weekday.empty()) {
                cout << "Invalid date.\n";
                break;
            }
            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), weekday);
            timetable.displayEntries(entries, "Timetable for " + date + " (" + weekday + ")");
            break;
        }
        case 3: {
            string date = selectDate(today);
            string weekday = DateTimeUtils::getWeekdayFromDate(date);

            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), weekday);

            cout << "\n+----------------------------------------------------------+\n";
            cout << "|           FREE PERIODS (" << date << " - " << weekday << ")          |\n";
            cout << "+----------------------------------------------------------+\n";

            if (entries.empty()) {
                cout << "| No classes scheduled. Free all day!                      |\n";
                cout << "+----------------------------------------------------------+\n";
                break;
            }

            auto freePeriods = findFreePeriods(entries);
            for (const auto& period : freePeriods) {
                int duration = period.second - period.first;
                cout << "| " << left << setw(8) << DateTimeUtils::minutesToTime(period.first)
                     << " - " << setw(8) << DateTimeUtils::minutesToTime(period.second)
                     << " | FREE (" << duration << " min) |\n";
            }
            if (freePeriods.empty()) {
                cout << "| No free periods between classes.                         |\n";
            }
            cout << "+----------------------------------------------------------+\n";
            break;
        }
        case 4:
        case 5:
        case 6:
            classroomAction(choice - 3, finder, today);
            break;
        case 7:
            routeMenu(student, timetable, navigator);
            break;
        case 8:
            assignmentMenu(assignments, assignmentFile);
            break;
        case 9: {
            string date = selectDate(today);
            string weekday = DateTimeUtils::getWeekdayFromDate(date);

            auto entries = timetable.getEntriesForStudent(
                student.getCourse(), student.getSemester(), student.getSection(), weekday);

            auto freePeriods = findFreePeriods(entries);

            if (freePeriods.empty()) {
                cout << "No free periods available.\n";
                break;
            }

            cout << "\nAvailable free periods:\n";
            for (size_t i = 0; i < freePeriods.size(); ++i) {
                int dur = freePeriods[i].second - freePeriods[i].first;
                cout << i + 1 << ". " << DateTimeUtils::minutesToTime(freePeriods[i].first) << " - "
                     << DateTimeUtils::minutesToTime(freePeriods[i].second) << " (" << dur
                     << " min)\n";
            }

            int periodChoice = getMenuChoice(1, freePeriods.size(), "Select free period: ");
            int availableMinutes =
                freePeriods[periodChoice - 1].second - freePeriods[periodChoice - 1].first;

            cout << "\nRecommendation mode:\n";
            cout << "1. Quick greedy recommendation\n";
            cout << "2. Optimized knapsack recommendation\n";
            int mode = getMenuChoice(1, 2);

            if (date != today)
                cout << "Task urgency is based on today; the selected date determines the free "
                        "period.\n";
            auto pending = assignments.getPendingAssignments();
            vector<Scheduler::Recommendation> recs;

            if (mode == 1) {
                recs = Scheduler::greedyRecommend(pending, availableMinutes);
            } else {
                recs = Scheduler::knapsackRecommend(pending, availableMinutes);
            }

            Scheduler::displayRecommendations(recs, availableMinutes);
            break;
        }
        case 10:
            student.displayProfile();
            break;
        case 0:
            if (assignments.saveToCSV(assignmentFile)) {
                cout << "Assignments saved. Returning to the main menu.\n";
                return;
            }
            break;
        }
        pause();
    }
}

void teacherMenu(ClassroomFinder& finder) {
    while (true) {
        displayTeacherMenu();
        int choice = getMenuChoice(0, 3);
        if (choice == 0)
            return;
        classroomAction(choice, finder, DateTimeUtils::getCurrentDate());
        pause();
    }
}
