#ifndef USER_H
#define USER_H

#include <string>

class User {
  protected:
    std::string name;

  public:
    User() {}
    User(const std::string& n) : name(n) {}
    virtual ~User() {}

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    virtual void displayProfile() const = 0;
    virtual std::string getRole() const = 0;
};

#endif