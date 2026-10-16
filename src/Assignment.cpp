#include "Assignment.h"
#include <iostream>

using namespace std;

void Assignment::display() const {
    cout << "ID " << id << " | " << title << " | " << subject
              << " | Due: " << dueDate << " | ";
    if (completed) {
        cout << "Completed";
    } else {
        cout << "Pending";
    }
    cout << '\n';
}
