#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H
#include <string>

using namespace std;


namespace InputValidator {
    // A small signal caught by main when the input stream closes.
    struct EndOfInput {};
    string getStringInput(const string& prompt);
    int getIntInput(const string& prompt, int minimum, int maximum);
    int getTimeInput();
    string getDateInput();
    string getDayInput();
}
#endif
