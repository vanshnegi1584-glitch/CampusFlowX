#ifndef ASSIGNMENT_MANAGER_H
#define ASSIGNMENT_MANAGER_H
#include "Assignment.h"
#include <string>
#include <vector>

using namespace std;


class AssignmentManager {
private:
    vector<Assignment> assignments;
    string filename;
    int nextId = 1;
    void saveToCSV() const;
public:
    void loadFromCSV(const string& path);
    void addAssignment(const string& title, const string& subject,
                       const string& dueDate);
    bool markCompleted(int id);
    void displayAll() const;
};
#endif
