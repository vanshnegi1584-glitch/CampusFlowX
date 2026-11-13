#include "User.h"
#include "Student.h"
#include "Teacher.h"
#include <iostream>
#include <iomanip>

void Student::displayProfile() const {
    std::cout << "\n"
              << name << " | Student\n"
              << course << " | Semester " << semester << " | Section " << section << '\n';
}

void Teacher::displayProfile() const {
    std::cout << "\n" << name << " | Teacher\n";
    if (!department.empty())
        std::cout << "Department: " << department << '\n';
}
