#ifndef TIMETABLE_H
#define TIMETABLE_H
#include "ScheduleEntry.h"
#include <string>
#include <vector>

using namespace std;


class Timetable {
private:
    vector<ScheduleEntry> entries;
public:
    void loadFromCSV(const string& filename);
    void displayAll() const;
    void searchSubject(const string& keyword) const;
    const vector<ScheduleEntry>& getEntries() const;
    vector<string> getAllRooms() const;
};
#endif
