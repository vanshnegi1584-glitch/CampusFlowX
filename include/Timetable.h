#ifndef TIMETABLE_H
#define TIMETABLE_H

#include "ScheduleEntry.h"
#include <vector>
#include <string>
#include <unordered_map>

class Timetable {
  private:
    std::vector<ScheduleEntry> allEntries;
    std::unordered_map<std::string, std::vector<ScheduleEntry>> entriesByKey;
    std::vector<std::string> allCourses;
    std::vector<std::string> allRooms;
    std::vector<std::string> allDays = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                                        "Friday", "Saturday", "Sunday"};
    bool debugMode = false;

    void buildIndex();
    void extractUniqueCourses();
    void extractUniqueRooms();

  public:
    Timetable() {}

    void setDebugMode(bool enabled) { debugMode = enabled; }
    bool getDebugMode() const { return debugMode; }

    bool loadFromCSV(const std::string& filename);
    bool saveToCSV(const std::string& filename) const;
    std::string resolveRoom(const std::string& room) const;
    std::vector<int> getSemesters(const std::string& course) const;
    std::vector<std::string> getSections(const std::string& course, int semester) const;

    std::vector<ScheduleEntry> getEntriesForStudent(const std::string& course, int semester,
                                                    const std::string& section,
                                                    const std::string& day) const;
    std::vector<ScheduleEntry> getEntriesForRoom(const std::string& room,
                                                 const std::string& day) const;
    std::vector<ScheduleEntry> getAllEntriesForDay(const std::string& day) const;

    const std::vector<std::string>& getAllCourses() const { return allCourses; }
    const std::vector<std::string>& getAllRooms() const { return allRooms; }
    const std::vector<std::string>& getAllDays() const { return allDays; }

    bool hasEntry(const std::string& course, int semester, const std::string& section,
                  const std::string& day) const;
    void addEntry(const ScheduleEntry& entry);
    void displayEntries(const std::vector<ScheduleEntry>& entries, const std::string& title) const;
};

#endif
