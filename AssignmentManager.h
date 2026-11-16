#ifndef ASSIGNMENT_MANAGER_H
#define ASSIGNMENT_MANAGER_H

#include "Assignment.h"
#include "Dsa.h"
#include <string>
#include <vector>

using namespace std;

using AssignmentNode = DsaListNode;

class AssignmentManager {
  private:
    AssignmentNode* head;
    int nextId;
    int count;

    void clearList();
    void copyList(const AssignmentNode* otherHead);
    AssignmentNode* findNode(int id) const;

  public:
    AssignmentManager();
    ~AssignmentManager();
    AssignmentManager(const AssignmentManager& other);
    AssignmentManager& operator=(const AssignmentManager& other);

    bool loadFromCSV(const string& filename);
    bool saveToCSV(const string& filename) const;

    void addAssignment(const Assignment& assignment);
    bool editAssignment(int id);
    bool deleteAssignment(int id);
    Assignment* findAssignment(int id);
    void displayAll() const;
    void displayPending() const;
    void sortByDeadline();
    vector<Assignment> getAllAssignments() const;
    vector<Assignment> getPendingAssignments() const;
    int getCount() const { return count; }
    int getNextId() const { return nextId; }
    void setNextId(int id) { nextId = id; }
};

#endif