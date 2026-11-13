#include "Timetable.h"
#include "DateTimeUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <stdexcept>

namespace {
using namespace DateTimeUtils;
int semesterNumber(const std::string& text) {
    int n = 0;
    if (parseInteger(text, n))
        return n >= 1 && n <= 12 ? n : -1;
    static const std::vector<std::string> roman = {"I",   "II",   "III", "IV", "V",  "VI",
                                                   "VII", "VIII", "IX",  "X",  "XI", "XII"};
    auto it = std::find(roman.begin(), roman.end(), normalizeSection(text));
    return it == roman.end() ? -1 : static_cast<int>(it - roman.begin()) + 1;
}
std::vector<std::string> sections(const std::string& text) {
    std::stringstream s(normalizeSection(text));
    std::string part;
    std::vector<std::string> result;
    while (std::getline(s, part, '+'))
        if (!part.empty())
            result.push_back(part);
    return result;
}
bool sectionMatches(const std::string& student, const std::string& entry) {
    auto a = sections(student), b = sections(entry);
    if (a.empty() || b.empty())
        return false;
    if (std::find(b.begin(), b.end(), "ALL") != b.end() ||
        std::find(a.begin(), a.end(), "ALL") != a.end())
        return true;
    for (const auto& s : a)
        if (std::find(b.begin(), b.end(), s) != b.end())
            return true;
    return false;
}
std::string normalizedVenue(const std::string& text) {
    auto rooms = splitRooms(text);
    if (rooms.empty())
        return "Venue not listed";
    if (std::all_of(rooms.begin(), rooms.end(), [](const auto& r) { return isValidClassroom(r); }))
        return normalizeRoom(text);
    return trim(text).empty() ? "Venue not listed" : trim(text);
}
bool entryLess(const ScheduleEntry& a, const ScheduleEntry& b) {
    if (a.getStartTime() != b.getStartTime())
        return a.getStartTime() < b.getStartTime();
    if (a.getEndTime() != b.getEndTime())
        return a.getEndTime() < b.getEndTime();
    return a.getSubject() < b.getSubject();
}
std::string entryKey(const ScheduleEntry& e) {
    std::ostringstream s;
    for (const auto& field :
         std::vector<std::string>{e.getCourse(), std::to_string(e.getSemester()), e.getSection(),
                                  e.getDay(), e.getSubject(), std::to_string(e.getStartTime()),
                                  std::to_string(e.getEndTime()), e.getRoomNumber()})
        s << field.size() << ':' << field;
    return s.str();
}
ScheduleEntry normalizedEntry(const ScheduleEntry& e) {
    auto course = normalizeCourse(e.getCourse()), day = normalizeDay(e.getDay()),
         section = normalizeSection(e.getSection());
    if (course.empty() || section.empty() || trim(e.getSubject()).empty() || !isValidDay(day) ||
        e.getSemester() < 1 || e.getSemester() > 12 || e.getStartTime() < COLLEGE_START_MINUTES ||
        e.getEndTime() > COLLEGE_END_MINUTES || e.getStartTime() >= e.getEndTime())
        throw std::invalid_argument("Invalid timetable entry");
    return {course,
            e.getSemester(),
            section,
            day,
            trim(e.getSubject()),
            e.getStartTime(),
            e.getEndTime(),
            normalizedVenue(e.getRoomNumber())};
}
} // namespace

void Timetable::buildIndex() {
    entriesByKey.clear();
    for (const auto& e : allEntries)
        entriesByKey[e.getCourse() + "|" + std::to_string(e.getSemester())].push_back(e);
    for (auto& pair : entriesByKey)
        std::sort(pair.second.begin(), pair.second.end(), entryLess);
}
void Timetable::extractUniqueCourses() {
    std::set<std::string> courses;
    for (const auto& e : allEntries)
        courses.insert(e.getCourse());
    allCourses.assign(courses.begin(), courses.end());
}
void Timetable::extractUniqueRooms() {
    std::set<std::string> rooms;
    for (const auto& e : allEntries)
        for (const auto& room : splitRooms(e.getRoomNumber()))
            if (isValidClassroom(room))
                rooms.insert(room);
    // Bare numbers alias a typed room only if the match is unambiguous.
    std::vector<std::string> raw(rooms.begin(), rooms.end());
    for (const auto& room : raw) {
        if (!std::all_of(room.begin(), room.end(), [](unsigned char c) { return std::isdigit(c); }))
            continue;
        int matches = 0;
        for (const auto& suffix : {"CR", "LT", "LAB", "AUDI"})
            if (rooms.count(room + suffix))
                ++matches;
        if (matches == 1)
            rooms.erase(room);
    }
    allRooms.assign(rooms.begin(), rooms.end());
}
std::string Timetable::resolveRoom(const std::string& room) const {
    const auto key = normalizeRoom(room);
    if (std::find(allRooms.begin(), allRooms.end(), key) != allRooms.end())
        return key;
    std::string match;
    for (const auto& suffix : {"CR", "LT", "LAB", "AUDI"}) {
        const auto candidate = key + suffix;
        if (std::find(allRooms.begin(), allRooms.end(), candidate) != allRooms.end()) {
            if (!match.empty())
                return key;
            match = candidate;
        }
    }
    return match.empty() ? key : match;
}

bool Timetable::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Cannot open timetable: " << filename << '\n';
        return false;
    }
    std::string line;
    std::vector<ScheduleEntry> loaded;
    std::map<std::string, size_t> columns;
    std::set<std::string> seen;
    size_t lineNumber = 0, invalid = 0, duplicates = 0;
    bool headerRead = false;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (trim(line).empty())
            continue;
        try {
            auto fields = parseCSVLine(line);
            if (!headerRead) {
                for (size_t i = 0; i < fields.size(); ++i) {
                    if (!columns.emplace(normalizeString(fields[i]), i).second)
                        throw std::invalid_argument("Duplicate header");
                }
                for (const auto& name : {"course", "semester", "section", "subject", "day",
                                         "start_time", "end_time", "room_number"})
                    if (!columns.count(name))
                        throw std::invalid_argument(std::string("Missing header: ") + name);
                headerRead = true;
                continue;
            }
            if (fields.size() != columns.size())
                throw std::invalid_argument("Wrong number of fields");
            auto value = [&](const std::string& key) { return fields.at(columns.at(key)); };
            ScheduleEntry e(value("course"), semesterNumber(value("semester")), value("section"),
                            value("day"), value("subject"), timeToMinutes(value("start_time")),
                            timeToMinutes(value("end_time")), value("room_number"));
            e = normalizedEntry(e);
            if (!seen.insert(entryKey(e)).second) {
                ++duplicates;
                continue;
            }
            loaded.push_back(e);
        } catch (const std::exception& e) {
            std::cerr << "Timetable line " << lineNumber << ": " << e.what() << '\n';
            if (!headerRead)
                return false;
            ++invalid;
        }
    }
    if (file.bad() || !headerRead || loaded.empty()) {
        std::cerr << "Timetable contains no usable schedule or could not be read.\n";
        return false;
    }
    allEntries.swap(loaded);
    buildIndex();
    extractUniqueCourses();
    extractUniqueRooms();
    std::cout << "Timetable: " << allEntries.size() << " classes, " << allCourses.size()
              << " courses, " << allRooms.size() << " rooms.\n";
    if (invalid || duplicates || debugMode)
        std::cout << "Skipped " << invalid << " invalid rows and " << duplicates
                  << " duplicate rows.\n";
    return true;
}
bool Timetable::saveToCSV(const std::string& filename) const {
    std::ostringstream file;
    file << "course,semester,section,day,subject,start_time,end_time,room_number\n";
    try {
        for (const auto& e : allEntries) {
            file << escapeCSVField(e.getCourse()) << ',' << e.getSemester() << ','
                 << escapeCSVField(e.getSection()) << ',' << e.getDay() << ','
                 << escapeCSVField(e.getSubject()) << ',' << minutesToTime(e.getStartTime()) << ','
                 << minutesToTime(e.getEndTime()) << ',' << escapeCSVField(e.getRoomNumber())
                 << '\n';
        }
        return atomicWriteFile(filename, file.str());
    } catch (const std::exception& e) {
        std::cerr << "Cannot save timetable: " << e.what() << '\n';
        return false;
    }
}
std::vector<ScheduleEntry> Timetable::getEntriesForStudent(const std::string& course, int semester,
                                                           const std::string& section,
                                                           const std::string& day) const {
    std::vector<ScheduleEntry> result;
    auto it = entriesByKey.find(normalizeCourse(course) + "|" + std::to_string(semester));
    if (it == entriesByKey.end() || !isValidDay(day))
        return result;
    const auto d = normalizeDay(day);
    for (const auto& e : it->second)
        if (e.getDay() == d && sectionMatches(section, e.getSection()))
            result.push_back(e);
    return result;
}
std::vector<ScheduleEntry> Timetable::getEntriesForRoom(const std::string& room,
                                                        const std::string& day) const {
    std::vector<ScheduleEntry> result;
    const auto key = resolveRoom(room), d = normalizeDay(day);
    for (const auto& e : allEntries) {
        if (e.getDay() != d)
            continue;
        for (const auto& r : splitRooms(e.getRoomNumber())) {
            if (resolveRoom(r) == key) {
                result.push_back(e);
                break;
            }
        }
    }
    std::sort(result.begin(), result.end(), entryLess);
    return result;
}
std::vector<ScheduleEntry> Timetable::getAllEntriesForDay(const std::string& day) const {
    std::vector<ScheduleEntry> result;
    for (const auto& e : allEntries)
        if (e.getDay() == normalizeDay(day))
            result.push_back(e);
    std::sort(result.begin(), result.end(), entryLess);
    return result;
}
std::vector<int> Timetable::getSemesters(const std::string& course) const {
    std::set<int> result;
    for (const auto& e : allEntries)
        if (e.getCourse() == normalizeCourse(course))
            result.insert(e.getSemester());
    return {result.begin(), result.end()};
}
std::vector<std::string> Timetable::getSections(const std::string& course, int semester) const {
    std::set<std::string> result;
    auto it = entriesByKey.find(normalizeCourse(course) + "|" + std::to_string(semester));
    if (it != entriesByKey.end()) {
        for (const auto& e : it->second) {
            for (const auto& s : sections(e.getSection())) {
                result.insert(s);
            }
        }
    }
    return {result.begin(), result.end()};
}
bool Timetable::hasEntry(const std::string& course, int semester, const std::string& section,
                         const std::string& day) const {
    return !getEntriesForStudent(course, semester, section, day).empty();
}
void Timetable::addEntry(const ScheduleEntry& entry) {
    const auto e = normalizedEntry(entry);
    const auto key = entryKey(e);
    for (const auto& existing : allEntries)
        if (entryKey(existing) == key)
            return;
    allEntries.push_back(e);
    buildIndex();
    extractUniqueCourses();
    extractUniqueRooms();
}
void Timetable::displayEntries(const std::vector<ScheduleEntry>& entries,
                               const std::string& title) const {
    std::cout << '\n' << title << '\n';
    if (entries.empty()) {
        std::cout << "No classes scheduled.\n";
        return;
    }
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        std::cout << i + 1 << ". " << minutesToTime(e.getStartTime()) << " - "
                  << minutesToTime(e.getEndTime()) << " | " << e.getSubject()
                  << "\n   Room: " << e.getRoomNumber() << '\n';
    }
}
void ScheduleEntry::display(int index) const {
    std::cout << index << ". " << DateTimeUtils::minutesToTime(startTime) << " - "
              << DateTimeUtils::minutesToTime(endTime) << " | " << subject
              << "\n   Room: " << roomNumber << '\n';
}
