#include "Timetable.h"
#include "DateTimeUtils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

// Keep empty cells so a missing room does not shift the other columns.
static vector<string> splitTimetableRow(const string& line) {
    vector<string> fields;
    string field;
    for (char letter : line) {
        if (letter == ',') {
            fields.push_back(DateTimeUtils::trim(field));
            field.clear();
        } else {
            field += letter;
        }
    }
    fields.push_back(DateTimeUtils::trim(field));
    for (const string& value : fields) {
        if (!value.empty() && !DateTimeUtils::isPlainText(value)) {
            throw runtime_error("Unsupported character in timetable field.");
        }
    }
    return fields;
}

void ScheduleEntry::display() const {
    cout << day << " | " << DateTimeUtils::minutesToTime(startTime) << "-"
              << DateTimeUtils::minutesToTime(endTime) << " | " << subject
              << " | Room: ";
    if (roomNumber.empty()) {
        cout << "Unknown venue";
    } else {
        cout << roomNumber;
    }
    if (!course.empty()) {
        cout << " | " << course << " | Semester: " << semester << " | Section: " << section;
    }
    cout << '\n';
}

void Timetable::loadFromCSV(const string& filename) {
    ifstream file(filename);
    string line;
    if (!getline(file, line)) {
        throw runtime_error("Cannot read timetable or its header: " + filename);
    }
    // Accept a UTF-8 BOM sometimes added by spreadsheet editors.
    if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        line.erase(0, 3);
    }
    string header = DateTimeUtils::trim(line);
    bool fullFormat = header == "course,semester,section,subject,day,start_time,end_time,room_number";
    if (!fullFormat && header != "day,subject,start_time,end_time,room_number") {
        throw runtime_error("Cannot read timetable or its header: " + filename);
    }
    vector<ScheduleEntry> loaded;
    int lineNumber = 1;
    while (getline(file, line)) {
        ++lineNumber;
        if (DateTimeUtils::trim(line).empty()) {
            continue;
        }
        vector<string> fields = splitTimetableRow(line);
        size_t expectedFields = 5;
        if (fullFormat) {
            expectedFields = 8;
        }
        if (fields.size() != expectedFields) {
            throw runtime_error("Timetable field count at line " + to_string(lineNumber));
        }
        ScheduleEntry entry;
        if (fullFormat) {
            entry.course = fields[0];
            entry.semester = fields[1];
            entry.section = fields[2];
            entry.subject = fields[3];
            entry.day = fields[4];
            entry.startTime = DateTimeUtils::timeToMinutes(fields[5]);
            entry.endTime = DateTimeUtils::timeToMinutes(fields[6]);
            entry.roomNumber = fields[7];
            if (entry.course.empty() || entry.semester.empty() || entry.section.empty()) {
                throw runtime_error("Missing course/semester/section at line " + to_string(lineNumber));
            }
        } else {
            entry.day = fields[0];
            entry.subject = fields[1];
            entry.startTime = DateTimeUtils::timeToMinutes(fields[2]);
            entry.endTime = DateTimeUtils::timeToMinutes(fields[3]);
            entry.roomNumber = fields[4];
        }
        if (entry.subject.empty() || !DateTimeUtils::isValidDay(entry.day) || entry.startTime < 0 ||
            entry.endTime <= entry.startTime) {
            throw runtime_error("Invalid timetable day/time at line " + to_string(lineNumber));
        }
        loaded.push_back(entry);
    }
    if (file.bad() || loaded.empty()) {
        throw runtime_error("Timetable is empty or unreadable.");
    }
    entries = loaded;
}

void Timetable::displayAll() const {
    cout << "\nLoaded timetable (" << entries.size() << " classes)\n";
    for (const ScheduleEntry& entry : entries) {
        entry.display();
    }
}

void Timetable::searchSubject(const string& keyword) const {
    bool found = false;
    string search = DateTimeUtils::lowerCase(keyword);
    for (const ScheduleEntry& entry : entries) {
        string subject = DateTimeUtils::lowerCase(entry.subject);
        if (subject.find(search) != string::npos) {
            entry.display();
            found = true;
        }
    }
    if (!found) {
        cout << "No matching class found.\n";
    }
}

const vector<ScheduleEntry>& Timetable::getEntries() const {
    return entries;
}

vector<string> Timetable::getAllRooms() const {
    vector<string> rooms;
    for (const ScheduleEntry& entry : entries) {
        if (entry.roomNumber.empty()) {
            continue;
        }
        bool alreadyListed = false;
        for (const string& room : rooms) {
            if (room == entry.roomNumber) {
                alreadyListed = true;
                break;
            }
        }
        if (!alreadyListed) {
            rooms.push_back(entry.roomNumber);
        }
    }
    return rooms;
}
