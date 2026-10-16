#ifndef DATE_TIME_UTILS_H
#define DATE_TIME_UTILS_H
#include <string>
#include <vector>

using namespace std;


namespace DateTimeUtils {
    string trim(const string& text);
    string lowerCase(string text);
    bool parseInteger(const string& text, int& value);
    int timeToMinutes(const string& text);
    string minutesToTime(int minutes);
    bool isValidDay(const string& day);
    bool isValidDate(const string& date);
    bool isPlainText(const string& text);
    // Deliberately simple CSV: commas and quotes inside fields are not supported.
    vector<string> parseCSVLine(const string& line);
}
#endif
