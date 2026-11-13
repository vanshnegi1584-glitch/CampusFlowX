#include "DateTimeUtils.h"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace DateTimeUtils {
std::string trim(const std::string& str) {
    const auto first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    return str.substr(first, str.find_last_not_of(" \t\n\r") - first + 1);
}
bool parseInteger(const std::string& str, int& value) {
    const auto text = trim(str);
    if (text.empty())
        return false;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    return result.ec == std::errc{} && result.ptr == text.data() + text.size();
}
std::string normalizeString(const std::string& str) {
    std::string result;
    for (unsigned char c : trim(str)) {
        if (std::isspace(c)) {
            if (!result.empty() && result.back() != ' ')
                result += ' ';
        } else
            result += static_cast<char>(std::tolower(c));
    }
    return result;
}
std::string normalizeCourse(const std::string& course) {
    std::string s = normalizeString(course);
    s = std::regex_replace(s, std::regex(R"([.()])"), "");
    s = std::regex_replace(s, std::regex("&"), " and ");
    s = normalizeString(s);
    for (const auto& pair :
         std::vector<std::pair<std::string, std::string>>{{"b tech", "btech"},
                                                          {"m tech", "mtech"},
                                                          {"b sc", "bsc"},
                                                          {"m sc", "msc"},
                                                          {"c s e", "cse"},
                                                          {"ai and ds", "ai ds"}}) {
        s = std::regex_replace(s, std::regex(pair.first), pair.second);
    }
    static const std::unordered_map<std::string, std::string> courses = {
        {"btech", "B.Tech"},
        {"btech cse", "B.Tech CSE"},
        {"btech cs", "B.Tech CSE"},
        {"btech ece", "B.Tech ECE"},
        {"btech mech", "B.Tech MECH"},
        {"mtech", "M.Tech"},
        {"bca", "BCA"},
        {"bca ai ds", "BCA AI & DS"},
        {"bca cs", "BCA CS"},
        {"mca", "MCA"},
        {"mca ai ds", "MCA (AI&DS)"},
        {"bsc", "B.Sc."},
        {"bsc it", "B.Sc. IT"},
        {"msc", "M.Sc."},
        {"diploma cs", "Diploma CS"}};
    auto it = courses.find(s);
    if (it != courses.end())
        return it->second;
    bool newWord = true;
    for (char& c : s) {
        if (newWord)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        newWord = c == ' ';
    }
    return s;
}
std::string normalizeSection(const std::string& section) {
    std::string result;
    for (unsigned char c : section)
        if (!std::isspace(c))
            result += static_cast<char>(std::toupper(c));
    return result;
}
std::string normalizeDay(const std::string& day) {
    static const std::vector<std::string> days = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                                                  "Friday", "Saturday", "Sunday"};
    const auto key = normalizeString(day);
    for (const auto& d : days) {
        const auto full = normalizeString(d);
        if (key == full || key == full.substr(0, 3))
            return d;
    }
    return trim(day);
}
bool isValidDay(const std::string& day) {
    const auto d = normalizeDay(day);
    return d == "Monday" || d == "Tuesday" || d == "Wednesday" || d == "Thursday" ||
           d == "Friday" || d == "Saturday" || d == "Sunday";
}
std::string normalizeRoom(const std::string& room) {
    if (room.find('/') != std::string::npos) {
        const auto rooms = splitRooms(room);
        std::string result;
        for (const auto& r : rooms) {
            if (!result.empty())
                result += " / ";
            result += r;
        }
        return result;
    }
    std::string s;
    for (unsigned char c : trim(room))
        if (!std::isspace(c) && c != '(' && c != ')')
            s += static_cast<char>(std::toupper(c));
    if (s == "NEWAUDI")
        return "529AUDI";
    std::smatch m;
    static const std::regex prefix(R"(^(CR|LT|AUDI)-?(\d+[A-Z]?)$)");
    static const std::regex lab(R"(^LAB-?(\d+(?:-[A-Z])?)$)");
    if (std::regex_match(s, m, prefix))
        return m[2].str() + m[1].str();
    if (std::regex_match(s, m, lab)) {
        auto number = m[1].str();
        return number.size() >= 3 ? number + "LAB" : "LAB" + number;
    }
    return s;
}
std::vector<std::string> splitRooms(const std::string& room) {
    std::vector<std::string> result;
    std::stringstream stream(room);
    std::string part;
    while (std::getline(stream, part, '/')) {
        part = trim(part);
        if (!part.empty())
            result.push_back(normalizeRoom(part));
    }
    // A shared suffix applies to bare numbers: 306/423 (LT).
    if (result.size() > 1) {
        std::smatch match;
        static const std::regex suffix(R"(^\d+(CR|LT|AUDI|LAB)$)");
        if (std::regex_match(result.back(), match, suffix)) {
            const std::string type = match[1].str();
            for (auto& r : result)
                if (std::all_of(r.begin(), r.end(),
                                [](unsigned char c) { return std::isdigit(c); }))
                    r += type;
        }
    }
    std::vector<std::string> unique;
    for (const auto& r : result)
        if (std::find(unique.begin(), unique.end(), r) == unique.end())
            unique.push_back(r);
    return unique;
}
bool isValidClassroom(const std::string& room) {
    static const std::regex physical(
        R"(^(\d+([A-Z]|-[A-Z])?(CR|LT|LAB|AUDI)?|LAB[0-9A-Z-]+|AUDI|LOGICLAB|MICROLAB|RR-E|PHARMACYBUILDING)$)");
    return std::regex_match(normalizeRoom(room), physical);
}
bool isSuspiciousRoom(const std::string& room) {
    static const std::regex suspicious(R"(^\d{1,2}(CR|LT)$)");
    return std::regex_match(normalizeRoom(room), suspicious);
}
std::string normalizeForDisplay(const std::string& str, const std::string& type) {
    if (type == "course")
        return normalizeCourse(str);
    if (type == "section")
        return normalizeSection(str);
    if (type == "day")
        return normalizeDay(str);
    if (type == "room")
        return normalizeRoom(str);
    return str;
}
bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    return normalizeString(a) == normalizeString(b);
}

int timeToMinutes(const std::string& time) {
    static const std::regex pattern(R"(^([0-9]{1,2})(?:[:.]([0-9]{2}))?\s*(am|pm)?$)",
                                    std::regex::icase);
    std::smatch match;
    const auto text = trim(time);
    if (!std::regex_match(text, match, pattern))
        return -1;
    int hour = std::stoi(match[1].str());
    const int minute = match[2].matched ? std::stoi(match[2].str()) : 0;
    if (minute > 59)
        return -1;
    if (match[3].matched) {
        if (hour < 1 || hour > 12)
            return -1;
        hour %= 12;
        if (normalizeString(match[3].str()) == "pm")
            hour += 12;
    } else {
        if (hour > 23)
            return -1;
        // College shorthand: 1-6 means afternoon; explicit AM selects early hours.
        if (hour >= 1 && hour <= 6)
            hour += 12;
    }
    return hour * 60 + minute;
}
int parseTimeInput(const std::string& time) {
    const int minutes = timeToMinutes(time);
    return minutes >= COLLEGE_START_MINUTES && minutes <= COLLEGE_END_MINUTES ? minutes : -1;
}
bool isValidTime(const std::string& time) {
    return timeToMinutes(time) >= 0;
}
bool isValidTimeInput(const std::string& time) {
    return parseTimeInput(time) >= 0;
}
std::string minutesToTime(int minutes) {
    minutes = std::max(0, minutes) % 1440;
    int hour = minutes / 60;
    std::ostringstream s;
    s << std::setfill('0') << std::setw(2) << (hour % 12 == 0 ? 12 : hour % 12) << ':'
      << std::setw(2) << minutes % 60 << (hour >= 12 ? " PM" : " AM");
    return s.str();
}
static std::tm localNow() {
    const auto now = std::time(nullptr);
    auto* local = std::localtime(&now);
    if (!local)
        throw std::runtime_error("Cannot read local date/time");
    return *local;
}
std::string getCurrentDate() {
    auto now = localNow();
    std::ostringstream s;
    s << std::put_time(&now, "%d-%m-%Y");
    return s.str();
}
std::string getCurrentWeekday() {
    return getWeekdayFromDate(getCurrentDate());
}
int getCurrentTimeMinutes() {
    auto now = localNow();
    return now.tm_hour * 60 + now.tm_min;
}
bool isLeapYear(int year) {
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}
int daysInMonth(int month, int year) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month >= 1 && month <= 12 ? days[month - 1] + (month == 2 && isLeapYear(year)) : 0;
}
bool isValidDate(const std::string& date) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-')
        return false;
    for (size_t i = 0; i < date.size(); ++i)
        if (i != 2 && i != 5 && !std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    int day = std::stoi(date.substr(0, 2)), month = std::stoi(date.substr(3, 2)),
        year = std::stoi(date.substr(6, 4));
    return year >= 1900 && year <= 2100 && day >= 1 && day <= daysInMonth(month, year);
}
static long long dateDayKey(const std::string& date) {
    int year = std::stoi(date.substr(6, 4)), month = std::stoi(date.substr(3, 2)),
        day = std::stoi(date.substr(0, 2));
    const int previousYear = year - 1;
    long long total =
        365LL * previousYear + previousYear / 4 - previousYear / 100 + previousYear / 400;
    for (int m = 1; m < month; ++m)
        total += daysInMonth(m, year);
    return total + day - 1;
}
std::string getWeekdayFromDate(const std::string& date) {
    if (!isValidDate(date))
        return "";
    static const char* names[] = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                                  "Friday", "Saturday", "Sunday"};
    return names[dateDayKey(date) % 7];
}
bool isDateInFuture(const std::string& date) {
    return isValidDate(date) && dateDayKey(date) > dateDayKey(getCurrentDate());
}
bool isValidDeadline(const std::string& deadline) {
    static const std::regex pattern(R"(^[0-9]{2}-[0-9]{2}-[0-9]{4} [0-9]{1,2}:[0-9]{2} (AM|PM)$)",
                                    std::regex::icase);
    return std::regex_match(deadline, pattern) && isValidDate(deadline.substr(0, 10)) &&
           isValidTime(deadline.substr(11));
}
long long deadlineMinuteKey(const std::string& deadline) {
    if (!isValidDeadline(deadline))
        throw std::invalid_argument("Invalid deadline: use DD-MM-YYYY HH:MM AM/PM");
    return dateDayKey(deadline.substr(0, 10)) * 1440 + timeToMinutes(deadline.substr(11));
}
int parseDeadlineToMinutes(const std::string& deadline) {
    if (!isValidDeadline(deadline))
        return -1;
    auto now = localNow();
    std::ostringstream date;
    date << std::put_time(&now, "%d-%m-%Y");
    return static_cast<int>(deadlineMinuteKey(deadline) - dateDayKey(date.str()) * 1440 -
                            now.tm_hour * 60 - now.tm_min);
}
std::string formatDeadline(const std::string& deadline) {
    return isValidDeadline(deadline)
               ? deadline.substr(0, 10) + " " + minutesToTime(timeToMinutes(deadline.substr(11)))
               : deadline;
}

std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false, closed = false;
    size_t start = line.compare(0, 3, "\xEF\xBB\xBF") == 0 ? 3 : 0;
    for (size_t i = start; i < line.size(); ++i) {
        const char c = line[i];
        if (quoted) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    quoted = false;
                    closed = true;
                }
            } else
                field += c;
        } else if (c == ',') {
            fields.push_back(closed ? field : trim(field));
            field.clear();
            closed = false;
        } else if (closed) {
            if (!std::isspace(static_cast<unsigned char>(c)))
                throw std::invalid_argument("Text after closing CSV quote");
        } else if (c == '"') {
            if (!trim(field).empty())
                throw std::invalid_argument("Quote inside unquoted CSV field");
            field.clear();
            quoted = true;
        } else
            field += c;
    }
    if (quoted)
        throw std::invalid_argument("Unclosed CSV quote");
    fields.push_back(closed ? field : trim(field));
    return fields;
}
std::string escapeCSVField(const std::string& field) {
    if (field.find_first_of("\r\n") != std::string::npos)
        throw std::invalid_argument("CSV fields must occupy one line");
    if (field.find_first_of(",\"") == std::string::npos && trim(field) == field)
        return field;
    std::string result = "\"";
    for (char c : field) {
        if (c == '"')
            result += '"';
        result += c;
    }
    return result + '"';
}
bool atomicWriteFile(const std::string& filename, const std::string& contents) {
    const std::filesystem::path target(filename), temp(filename + ".tmp");
    std::error_code ec;
    if (std::filesystem::exists(temp, ec) || std::filesystem::is_symlink(temp, ec) ||
        std::filesystem::is_directory(target, ec) || std::filesystem::is_symlink(target, ec))
        return false;
    std::ofstream file(temp, std::ios::binary | std::ios::trunc);
    if (!file)
        return false;
    file << contents;
    file.flush();
    bool ok = static_cast<bool>(file);
    file.close();
    ok = ok && !file.fail();
    if (ok) {
#ifdef _WIN32
        ok = MoveFileExW(temp.c_str(), target.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
        std::filesystem::rename(temp, target, ec);
        ok = !ec;
#endif
    }
    if (!ok)
        std::filesystem::remove(temp, ec);
    return ok;
}
} // namespace DateTimeUtils
