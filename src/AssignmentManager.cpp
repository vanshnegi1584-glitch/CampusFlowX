#include "AssignmentManager.h"
#include "DateTimeUtils.h"
#include <climits>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

void AssignmentManager::loadFromCSV(const string& path) {
    ifstream file(path);
    string line;
    if (!getline(file, line) || DateTimeUtils::trim(line) !=
        "id,title,subject,due_date,completed") {
        throw runtime_error("Cannot read assignments or their header: " + path);
    }
    vector<Assignment> loaded;
    int loadedNextId = 1;
    while (getline(file, line)) {
        if (DateTimeUtils::trim(line).empty()) {
            continue;
        }
        vector<string> fields = DateTimeUtils::parseCSVLine(line);
        Assignment assignment;
        if (fields.size() != 5) {
            throw runtime_error("An assignment row must have five fields.");
        }
        if (!DateTimeUtils::parseInteger(fields[0], assignment.id) ||
            assignment.id < 1 || assignment.id >= INT_MAX) {
            throw runtime_error("Invalid assignment ID.");
        }
        if (!DateTimeUtils::isValidDate(fields[3])) {
            throw runtime_error("Invalid assignment due date.");
        }
        if (fields[4] != "0" && fields[4] != "1") {
            throw runtime_error("Assignment completion must be 0 or 1.");
        }
        for (const Assignment& existing : loaded) {
            if (existing.id == assignment.id) {
                throw runtime_error("Duplicate assignment ID.");
            }
        }
        assignment.title = fields[1];
        assignment.subject = fields[2];
        assignment.dueDate = fields[3];
        assignment.completed = fields[4] == "1";
        loaded.push_back(assignment);
        if (assignment.id >= loadedNextId) {
            loadedNextId = assignment.id + 1;
        }
    }
    if (file.bad()) {
        throw runtime_error("Could not finish reading assignments.");
    }
    // Commit only after every record has been checked.
    assignments = loaded;
    nextId = loadedNextId;
    filename = path;
}

void AssignmentManager::saveToCSV() const {
    // Write the new content first, then replace the old file with a backup available.
    // One running app instance per project folder is supported.
    const string temporary = filename + ".tmp";
    const string backup = filename + ".bak";
    if (filesystem::exists(temporary) || filesystem::exists(backup)) {
        throw runtime_error("Save stopped: inspect existing assignments.csv.tmp/.bak files.");
    }
    ofstream file(temporary);
    file << "id,title,subject,due_date,completed\n";
    for (const Assignment& assignment : assignments) {
        file << assignment.id << ',' << assignment.title << ',' << assignment.subject << ','
             << assignment.dueDate << ',';
        if (assignment.completed) {
            file << 1;
        } else {
            file << 0;
        }
        file << '\n';
    }
    file.close();
    if (!file) {
        throw runtime_error("Could not write assignments. Previous file kept.");
    }
    filesystem::rename(filename, backup);
    error_code error;
    filesystem::rename(temporary, filename, error);
    if (error) {
        error_code restoreError;
        filesystem::rename(backup, filename, restoreError);
        throw runtime_error("Save failed. Old data is in assignments.csv or assignments.csv.bak.");
    }
    filesystem::remove(backup, error);
    if (error) {
        cerr << "Saved, but the old .bak file needs manual removal before another save.\n";
    }
}

void AssignmentManager::addAssignment(const string& title, const string& subject,
                                      const string& dueDate) {
    if (!DateTimeUtils::isPlainText(title) || !DateTimeUtils::isPlainText(subject) ||
        !DateTimeUtils::isValidDate(dueDate) || nextId >= INT_MAX) {
        throw runtime_error("Invalid assignment fields or no IDs remaining.");
    }
    Assignment assignment;
    assignment.id = nextId;
    assignment.title = title;
    assignment.subject = subject;
    assignment.dueDate = dueDate;
    assignments.push_back(assignment);
    try {
        saveToCSV();
    } catch (...) {
        assignments.pop_back(); // Undo the in-memory addition if saving fails.
        throw;
    }
    ++nextId;
}

bool AssignmentManager::markCompleted(int id) {
    for (Assignment& assignment : assignments) {
        if (assignment.id != id) {
            continue;
        }
        if (assignment.completed) {
            return true;
        }
        assignment.completed = true;
        try {
            saveToCSV();
        } catch (...) {
            assignment.completed = false;
            throw;
        }
        return true;
    }
    return false;
}

void AssignmentManager::displayAll() const {
    if (assignments.empty()) {
        cout << "No assignments yet.\n";
    }
    for (const Assignment& assignment : assignments) {
        assignment.display();
    }
}
