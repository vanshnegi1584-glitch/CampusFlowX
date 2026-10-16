#ifndef SCHEDULE_ENTRY_H
#define SCHEDULE_ENTRY_H
#include <string>

using namespace std;


// A single class in the weekly timetable.
class ScheduleEntry {
public:
    string course;
    string semester;
    string section;
    string day;
    string subject;
    int startTime = 0; // Minutes after midnight.
    int endTime = 0;
    string roomNumber;
    void display() const;
};
#endif
