#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H
#include <string>

using namespace std;


class Assignment {
public:
    int id = 0;
    string title;
    string subject;
    string dueDate;
    bool completed = false;
    void display() const;
};
#endif
