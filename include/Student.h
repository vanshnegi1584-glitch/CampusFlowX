#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"
#include <string>
#include <vector>
#include <algorithm>
#include "ScheduleEntry.h"

class Student : public User {
  private:
    std::string course;
    int semester;
    std::string section;

  public:
    Student() : semester(0) {}
    Student(const std::string& n, const std::string& c, int sem, const std::string& sec)
        : User(n), course(c), semester(sem), section(sec) {}

    const std::string& getCourse() const { return course; }
    int getSemester() const { return semester; }
    const std::string& getSection() const { return section; }

    void setCourse(const std::string& c) { course = c; }
    void setSemester(int s) { semester = s; }
    void setSection(const std::string& s) { section = s; }

    std::string getKey() const {
        std::string key = course + "-" + std::to_string(semester) + "-" + section;
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        return key;
    }

    void displayProfile() const override;
    std::string getRole() const override { return "Student"; }
};

#endif