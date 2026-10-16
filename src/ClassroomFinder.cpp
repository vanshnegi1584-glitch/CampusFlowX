#include "ClassroomFinder.h"
#include "DateTimeUtils.h"
#include <iostream>

using namespace std;

ClassroomFinder::ClassroomFinder(const Timetable& timetable) : timetable(timetable) {}

void ClassroomFinder::displayRooms() const {
    cout << "Rooms in the timetable (use the displayed spelling):\n";
    for (const string& room : timetable.getAllRooms()) {
        cout << "  " << room << '\n';
    }
}

void ClassroomFinder::checkRoomAvailability(const string& room, const string& day,
                                           int time) const {
    if (!DateTimeUtils::isValidDay(day) || time < 0 || time >= 24 * 60) {
        cout << "Invalid day or time.\n";
        return;
    }
    bool knownRoom = false;
    bool occupied = false;
    for (const ScheduleEntry& entry : timetable.getEntries()) {
        if (entry.roomNumber.empty()) {
            continue;
        }
        if (DateTimeUtils::lowerCase(entry.roomNumber) != DateTimeUtils::lowerCase(room)) {
            continue;
        }
        knownRoom = true;
        // A class occupies its start time, but not its end time.
        if (DateTimeUtils::lowerCase(entry.day) == DateTimeUtils::lowerCase(day) &&
            entry.startTime <= time && time < entry.endTime) {
            cout << "Occupied: ";
            entry.display();
            occupied = true;
        }
    }
    if (!knownRoom) {
        cout << "Unknown classroom. Choose a listed room.\n";
    }
    else if (!occupied) {
        cout << "Available in the loaded timetable at that time.\n";
    }
    cout << "Based on listed rooms only; missing venues cannot be checked. Not a live booking check.\n";
}
