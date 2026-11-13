#ifndef DATETIME_UTILS_H
#define DATETIME_UTILS_H

#include <string>
#include <ctime>
#include <chrono>
#include <vector>

namespace DateTimeUtils {
std::string trim(const std::string& str);
bool parseInteger(const std::string& str, int& value);
const int COLLEGE_START_MINUTES = 8 * 60;
const int COLLEGE_END_MINUTES = 18 * 60;
const int WALKING_SPEED_M_PER_MIN = 80;

// Normalization utilities
std::string normalizeString(const std::string& str);
std::string normalizeCourse(const std::string& course);
std::string normalizeSection(const std::string& section);
std::string normalizeDay(const std::string& day);
std::string normalizeRoom(const std::string& room);
std::vector<std::string> splitRooms(const std::string& room);
bool isValidDay(const std::string& day);
bool isValidClassroom(const std::string& room);
bool isSuspiciousRoom(const std::string& room);
std::string normalizeForDisplay(const std::string& str, const std::string& type = "");
bool equalsIgnoreCase(const std::string& a, const std::string& b);

// Time parsing (auto AM/PM based on college hours 8AM-6PM)
int parseTimeInput(const std::string& time);
int timeToMinutes(const std::string& time);
std::string minutesToTime(int minutes);
bool isValidTime(const std::string& time);
bool isValidTimeInput(const std::string& time);

// Date utilities
std::string getCurrentDate();
std::string getCurrentWeekday();
int getCurrentTimeMinutes();
bool isValidDate(const std::string& date);
std::string getWeekdayFromDate(const std::string& date);
bool isLeapYear(int year);
int daysInMonth(int month, int year);
bool isDateInFuture(const std::string& date);
int parseDeadlineToMinutes(const std::string& deadline);
bool isValidDeadline(const std::string& deadline);
long long deadlineMinuteKey(const std::string& deadline);
std::string formatDeadline(const std::string& deadline);

// CSV parsing utilities
std::vector<std::string> parseCSVLine(const std::string& line);
std::string escapeCSVField(const std::string& field);
bool atomicWriteFile(const std::string& filename, const std::string& contents);
} // namespace DateTimeUtils

#endif
