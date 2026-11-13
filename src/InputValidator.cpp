#include "InputValidator.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>

namespace InputValidator {
void clearInputBuffer() {
    if (std::cin.eof())
        throw EndOfInput();
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
std::string getStringInput(const std::string& prompt, bool allowEmpty) {
    while (true) {
        std::cout << prompt << std::flush;
        std::string input;
        if (!std::getline(std::cin, input))
            throw EndOfInput();
        input = DateTimeUtils::trim(input);
        if (std::any_of(input.begin(), input.end(),
                        [](unsigned char c) { return std::iscntrl(c); })) {
            std::cout << "Please enter text without control characters.\n";
            continue;
        }
        if (allowEmpty || !input.empty())
            return input;
        std::cout << "Input cannot be empty. Please try again.\n";
    }
}
void pause() {
    getStringInput("\nPress Enter to continue...", true);
}
int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        int value;
        if (!DateTimeUtils::parseInteger(getStringInput(prompt), value)) {
            std::cout << "Invalid input. Please enter a whole number.\n";
        } else if (value < minVal || value > maxVal) {
            std::cout << "Value must be between " << minVal << " and " << maxVal << ".\n";
        } else
            return value;
    }
}
int getIntInputWithDefault(const std::string& prompt, int currentValue, int minVal, int maxVal) {
    while (true) {
        auto input = getStringInput(prompt, true);
        int value = currentValue;
        if (!input.empty() && !DateTimeUtils::parseInteger(input, value)) {
            std::cout << "Invalid input. Please enter a whole number.\n";
        } else if (value < minVal || value > maxVal) {
            std::cout << "Value must be between " << minVal << " and " << maxVal << ".\n";
        } else
            return value;
    }
}
std::string getNonEmptyStringInput(const std::string& prompt) {
    return getStringInput(prompt);
}
char getCharInput(const std::string& prompt, const std::string& validChars) {
    while (true) {
        const auto input = getStringInput(prompt);
        char c = static_cast<char>(std::toupper(static_cast<unsigned char>(input[0])));
        if (input.size() == 1 && (validChars.empty() || validChars.find(c) != std::string::npos))
            return c;
        std::cout << "Enter one character. Valid options: " << validChars << "\n";
    }
}
std::string getTimeInput(const std::string& prompt) {
    while (true) {
        auto input = getStringInput(prompt);
        if (DateTimeUtils::isValidTimeInput(input))
            return input;
        std::cout << "Enter a time from 08:00 AM to 06:00 PM (e.g., 8, 1:30, 13:30, 5 PM).\n";
    }
}
std::string getDateInput(const std::string& prompt, bool allowEmpty) {
    while (true) {
        auto input = getStringInput(prompt, allowEmpty);
        if ((allowEmpty && input.empty()) || DateTimeUtils::isValidDate(input))
            return input;
        std::cout << "Enter a valid date as DD-MM-YYYY (1900-2100).\n";
    }
}
std::string getSectionInput(const std::string& prompt) {
    while (true) {
        auto input = DateTimeUtils::normalizeSection(getStringInput(prompt));
        bool valid = !input.empty() && input.front() != '+' && input.back() != '+' &&
                     input.find("++") == std::string::npos;
        for (unsigned char c : input)
            if (!std::isalnum(c) && c != '-' && c != '+')
                valid = false;
        if (valid)
            return input;
        std::cout << "Enter a section such as A, A1, AI-1, All, or A+B+C.\n";
    }
}
int getMenuChoice(int minChoice, int maxChoice, const std::string& prompt) {
    return getIntInput(prompt, minChoice, maxChoice);
}
bool getYesNoInput(const std::string& prompt) {
    return getCharInput(prompt + " (Y/N): ", "YN") == 'Y';
}
std::string getValidCourseInput(const std::string& prompt,
                                const std::vector<std::string>& validCourses) {
    if (validCourses.empty())
        throw std::runtime_error("No courses are available in the timetable.");
    while (true) {
        auto input = DateTimeUtils::normalizeCourse(getStringInput(prompt));
        for (const auto& c : validCourses)
            if (DateTimeUtils::normalizeCourse(c) == input)
                return c;
        std::cout << "Available courses:\n";
        for (const auto& c : validCourses)
            std::cout << "  - " << c << '\n';
    }
}
int getValidSemesterInput(const std::string& prompt, int minSem, int maxSem) {
    return getIntInput(prompt, minSem, maxSem);
}
} // namespace InputValidator
