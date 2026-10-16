#include "DateTimeUtils.h"
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace DateTimeUtils {
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

string lowerCase(string text) {
    for (char& letter : text) {
        // unsigned char keeps character conversion safe for non-ASCII input.
        letter = static_cast<char>(tolower(static_cast<unsigned char>(letter)));
    }
    return text;
}

bool parseInteger(const string& text, int& value) {
    if (text.empty()) {
        return false;
    }
    for (char digit : text) {
        if (digit < '0' || digit > '9') {
            return false;
        }
    }
    istringstream input(text);
    if (input >> value) {
        return true;
    }
    return false; // The number was too large for an int.
}

int timeToMinutes(const string& text) {
    string time = lowerCase(trim(text));
    string period;
    if (time.size() == 8 && time[5] == ' ') {
        period = time.substr(6);
        if (period != "am" && period != "pm") {
            return -1;
        }
        time = time.substr(0, 5);
    }
    if (time.size() != 5 || time[2] != ':') {
        return -1;
    }
    int hours, minutes;
    if (!parseInteger(time.substr(0, 2), hours) ||
        !parseInteger(time.substr(3, 2), minutes)) {
        return -1;
    }
    if (hours > 23 || minutes > 59) {
        return -1;
    }
    if (!period.empty()) {
        if (hours < 1 || hours > 12) {
            return -1;
        }
        // 12 AM is midnight; 12 PM is noon.
        hours = hours % 12;
        if (period == "pm") {
            hours += 12;
        }
    }
    return hours * 60 + minutes;
}

string minutesToTime(int minutes) {
    string hours = to_string(minutes / 60);
    string remainingMinutes = to_string(minutes % 60);
    if (hours.size() == 1) {
        hours = "0" + hours;
    }
    if (remainingMinutes.size() == 1) {
        remainingMinutes = "0" + remainingMinutes;
    }
    return hours + ":" + remainingMinutes;
}

bool isValidDay(const string& day) {
    string days[] = {"monday", "tuesday", "wednesday", "thursday",
                     "friday", "saturday", "sunday"};
    string input = lowerCase(day);
    for (const string& name : days) {
        if (input == name) {
            return true;
        }
    }
    return false;
}

bool isValidDate(const string& date) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-') {
        return false;
    }
    int day, month, year;
    if (!parseInteger(date.substr(0, 2), day) ||
        !parseInteger(date.substr(3, 2), month) ||
        !parseInteger(date.substr(6, 4), year)) {
        return false;
    }
    if (year < 1900 || year > 2100 || month < 1 || month > 12) {
        return false;
    }
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
        days[1] = 29;
    }
    return day >= 1 && day <= days[month - 1];
}

bool isPlainText(const string& text) {
    if (trim(text).empty()) {
        return false;
    }
    for (unsigned char letter : text) {
        if (letter == ',' || letter == '"' || iscntrl(letter)) {
            return false;
        }
    }
    return true;
}

vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    istringstream input(line);
    string field;
    while (getline(input, field, ',')) {
        field = trim(field);
        if (!isPlainText(field)) {
            throw runtime_error("Empty or unsupported CSV field.");
        }
        fields.push_back(field);
    }
    if (!line.empty() && line.back() == ',') {
        throw runtime_error("Empty final CSV field.");
    }
    return fields;
}
}
