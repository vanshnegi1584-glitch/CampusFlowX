#include "InputValidator.h"
#include "DateTimeUtils.h"
#include <iostream>

using namespace std;

namespace InputValidator {
string getStringInput(const string& prompt) {
    while (true) {
        cout << prompt;
        string text;
        if (!getline(cin, text)) {
            throw EndOfInput{};
        }
        text = DateTimeUtils::trim(text);
        if (DateTimeUtils::isPlainText(text)) {
            return text;
        }
        cout << "Enter non-empty text without commas, quotes or control characters.\n";
    }
}

int getIntInput(const string& prompt, int minimum, int maximum) {
    while (true) {
        int value;
        if (DateTimeUtils::parseInteger(getStringInput(prompt), value) &&
            value >= minimum && value <= maximum) {
            return value;
        }
        cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
    }
}

int getTimeInput() {
    while (true) {
        int time = DateTimeUtils::timeToMinutes(getStringInput("Time (24-hour HH:MM): "));
        if (time >= 0) {
            return time;
        }
        cout << "Use 00:00 to 23:59, for example 08:30 or 14:10.\n";
    }
}

string getDateInput() {
    while (true) {
        string date = getStringInput("Due date (DD-MM-YYYY): ");
        if (DateTimeUtils::isValidDate(date)) {
            return date;
        }
        cout << "Enter a real date in years 1900 to 2100.\n";
    }
}

string getDayInput() {
    while (true) {
        string day = getStringInput("Weekday (for example Monday): ");
        if (DateTimeUtils::isValidDay(day)) {
            return day;
        }
        cout << "Enter a full weekday name, Monday through Sunday.\n";
    }
}
}
