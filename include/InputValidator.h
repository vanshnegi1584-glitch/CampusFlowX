#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>
#include <vector>
#include <climits>
#include <stdexcept>

namespace InputValidator {
class EndOfInput : public std::runtime_error {
  public:
    EndOfInput() : std::runtime_error("Input closed") {}
};
int getIntInput(const std::string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX);
int getIntInputWithDefault(const std::string& prompt, int currentValue, int minVal = 1,
                           int maxVal = INT_MAX);
std::string getStringInput(const std::string& prompt, bool allowEmpty = false);
std::string getNonEmptyStringInput(const std::string& prompt);
char getCharInput(const std::string& prompt, const std::string& validChars = "");
std::string getTimeInput(const std::string& prompt);
std::string getDateInput(const std::string& prompt, bool allowEmpty = false);
std::string getSectionInput(const std::string& prompt);
int getMenuChoice(int minChoice, int maxChoice, const std::string& prompt = "Enter choice: ");
bool getYesNoInput(const std::string& prompt);
std::string getValidCourseInput(const std::string& prompt,
                                const std::vector<std::string>& validCourses);
int getValidSemesterInput(const std::string& prompt, int minSem = 1, int maxSem = 8);
void clearInputBuffer();
void pause();
} // namespace InputValidator

#endif
