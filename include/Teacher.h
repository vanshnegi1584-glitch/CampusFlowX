#ifndef TEACHER_H
#define TEACHER_H

#include "User.h"
#include <string>

class Teacher : public User {
  private:
    std::string department;

  public:
    Teacher() {}
    Teacher(const std::string& n, const std::string& dept = "") : User(n), department(dept) {}

    const std::string& getDepartment() const { return department; }
    void setDepartment(const std::string& d) { department = d; }

    void displayProfile() const override;
    std::string getRole() const override { return "Teacher"; }
};

#endif