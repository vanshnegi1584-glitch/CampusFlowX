#include "AssignmentManager.h"
#include "InputValidator.h"
#include "DateTimeUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <set>
#include <climits>
#include <utility>
#include <memory>

namespace {
Assignment& assignmentData(const AssignmentNode* node) {
    return *static_cast<Assignment*>(node->value);
}

int matchesId(const void* value, const void* key) {
    return static_cast<const Assignment*>(value)->getId() == *static_cast<const int*>(key);
}

void deleteAssignmentValue(void* value) {
    delete static_cast<Assignment*>(value);
}

void insertAssignment(AssignmentNode** link, const Assignment& assignment) {
    auto value = std::make_unique<Assignment>(assignment);
    if (dsa_list_insert(link, value.get()) != DSA_OK)
        throw std::bad_alloc();
    value.release(); // The list now owns this C++ object through its cleanup callback.
}
} // namespace

using namespace InputValidator;
AssignmentManager::AssignmentManager() : head(nullptr), nextId(1), count(0) {}
AssignmentManager::~AssignmentManager() {
    clearList();
}
void AssignmentManager::clearList() {
    dsa_list_clear(&head, deleteAssignmentValue);
    count = 0;
}
void AssignmentManager::copyList(const AssignmentNode* other) {
    AssignmentManager copy;
    AssignmentNode** tail = &copy.head;
    while (other) {
        insertAssignment(tail, assignmentData(other));
        tail = &(*tail)->next;
        ++copy.count;
        other = other->next;
    }
    std::swap(head, copy.head);
    std::swap(count, copy.count);
}
AssignmentManager::AssignmentManager(const AssignmentManager& other)
    : head(nullptr), nextId(other.nextId), count(0) {
    copyList(other.head);
}
AssignmentManager& AssignmentManager::operator=(const AssignmentManager& other) {
    if (this != &other) {
        AssignmentManager copy(other);
        std::swap(head, copy.head);
        std::swap(count, copy.count);
        std::swap(nextId, copy.nextId);
    }
    return *this;
}
AssignmentNode* AssignmentManager::findNode(int id) const {
    return dsa_list_find(head, matchesId, &id);
}
bool AssignmentManager::loadFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file)
        return false;
    AssignmentManager loaded;
    AssignmentNode** tail = &loaded.head;
    set<int> ids;
    string line;
    size_t lineNumber = 0;
    bool header = false, invalid = false;
    while (getline(file, line)) {
        ++lineNumber;
        if (DateTimeUtils::trim(line).empty())
            continue;
        try {
            if (!header) {
                const vector<string> expected = {
                    "id",       "title",  "subject",          "deadline", "estimated_duration",
                    "priority", "status", "completed_minutes"};
                auto fields = DateTimeUtils::parseCSVLine(line);
                if (fields.size() != expected.size())
                    throw invalid_argument("Invalid assignment header");
                for (size_t i = 0; i < fields.size(); ++i)
                    if (!DateTimeUtils::equalsIgnoreCase(fields[i], expected[i]))
                        throw invalid_argument("Invalid assignment header");
                header = true;
                continue;
            }
            auto a = Assignment::fromCSV(line);
            if (!ids.insert(a.getId()).second)
                throw invalid_argument("Duplicate assignment ID");
            insertAssignment(tail, a);
            tail = &(*tail)->next;
            ++loaded.count;
            loaded.nextId = max(loaded.nextId, a.getId() + 1);
        } catch (const std::exception& e) {
            cerr << "Assignment line " << lineNumber << ": " << e.what() << '\n';
            if (!header)
                return false;
            invalid = true;
        }
    }
    // Do not replace data with a partial load that could erase rejected records on save.
    if (!header || invalid || file.bad())
        return false;
    std::swap(head, loaded.head);
    std::swap(count, loaded.count);
    std::swap(nextId, loaded.nextId);
    return true;
}
bool AssignmentManager::saveToCSV(const string& filename) const {
    ostringstream file;
    file << "id,title,subject,deadline,estimated_duration,priority,status,completed_minutes\n";
    try {
        for (auto* node = head; node; node = node->next)
            file << assignmentData(node).toCSV() << '\n';
        if (DateTimeUtils::atomicWriteFile(filename, file.str()))
            return true;
    } catch (const std::exception& e) {
        cerr << e.what() << '\n';
    }
    cerr << "Could not save assignments to " << filename << ". The previous file was preserved.\n";
    return false;
}
void AssignmentManager::addAssignment(const Assignment& assignment) {
    if (!assignment.isValid() || findNode(assignment.getId()))
        throw invalid_argument("Invalid or duplicate assignment ID");
    Assignment a = assignment;
    a.reconcileProgress();
    insertAssignment(&head, a);
    ++count;
    nextId = max(nextId, a.getId() + 1);
}
bool AssignmentManager::editAssignment(int id) {
    auto* node = findNode(id);
    if (!node)
        return false;
    Assignment a = assignmentData(node);
    cout << "\nEditing ID " << id << ". Leave any field blank to keep it.\n";
    auto title = getStringInput("Title [" + a.getTitle() + "]: ", true);
    if (!title.empty())
        a.setTitle(title);
    auto subject = getStringInput("Subject [" + a.getSubject() + "]: ", true);
    if (!subject.empty())
        a.setSubject(subject);
    while (true) {
        auto deadline =
            getStringInput("Deadline (DD-MM-YYYY HH:MM AM/PM) [" + a.getDeadline() + "]: ", true);
        if (deadline.empty())
            break;
        if (DateTimeUtils::isValidDeadline(deadline)) {
            a.setDeadline(DateTimeUtils::formatDeadline(deadline));
            break;
        }
        cout << "Invalid deadline. Please try again.\n";
    }
    a.setEstimatedDuration(getIntInputWithDefault("Estimated duration (minutes) [" +
                                                      to_string(a.getEstimatedDuration()) + "]: ",
                                                  a.getEstimatedDuration(), 1, 10000));
    auto optionalChoice = [](const string& prompt, char current, const string& choices) {
        while (true) {
            auto input = getStringInput(prompt, true);
            if (input.empty())
                return current;
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(input[0])));
            if (input.size() == 1 && choices.find(c) != string::npos)
                return c;
            cout << "Enter one of " << choices << " or leave blank.\n";
        }
    };
    auto p =
        optionalChoice("Priority (H/M/L) [" + a.getPriority() + "]: ", a.getPriority()[0], "HML");
    a.setPriority(p == 'H' ? "High" : p == 'M' ? "Medium" : "Low");
    auto s = optionalChoice("Status (P/I/C) [" + a.getStatus() + "]: ", a.getStatus()[0], "PIC");
    a.setStatus(s == 'P' ? "Pending" : s == 'I' ? "In Progress" : "Completed");
    if (s == 'C')
        a.setCompletedMinutes(a.getEstimatedDuration());
    else {
        // Reopening a completed task starts its progress at zero unless entered explicitly.
        int current = assignmentData(node).getStatus() == "Completed"
                          ? 0
                          : min(a.getCompletedMinutes(), a.getEstimatedDuration());
        a.setCompletedMinutes(
            getIntInputWithDefault("Minutes completed [" + to_string(current) + "]: ", current, 0,
                                   a.getEstimatedDuration()));
    }
    a.reconcileProgress();
    assignmentData(node) = a; // Commit only after every prompt succeeds, including on EOF.
    cout << "Assignment updated.\n";
    return true;
}
bool AssignmentManager::deleteAssignment(int id) {
    auto* node = findNode(id);
    if (!node)
        return false;
    deleteAssignmentValue(dsa_list_remove(&head, node));
    --count;
    return true;
}
Assignment* AssignmentManager::findAssignment(int id) {
    auto* node = findNode(id);
    return node ? &assignmentData(node) : nullptr;
}
void AssignmentManager::displayAll() const {
    if (!head) {
        cout << "\nNo assignments yet. Choose Add assignment to create one.\n";
        return;
    }
    cout << "\nAssignments (" << count << ")\n";
    for (auto* node = head; node; node = node->next)
        assignmentData(node).display();
}
void AssignmentManager::displayPending() const {
    bool found = false;
    for (auto* node = head; node; node = node->next)
        if (assignmentData(node).getStatus() != "Completed" &&
            assignmentData(node).getRemainingMinutes() > 0) {
            assignmentData(node).display();
            found = true;
        }
    if (!found)
        cout << "\nNo pending assignments.\n";
}
void AssignmentManager::sortByDeadline() {
    // Parse deadlines before sorting so a parsing error cannot reorder the list.
    for (auto* node = head; node; node = node->next)
        node->sort_key = DateTimeUtils::deadlineMinuteKey(assignmentData(node).getDeadline());
    dsa_list_sort(&head);
}
vector<Assignment> AssignmentManager::getAllAssignments() const {
    vector<Assignment> result;
    for (auto* node = head; node; node = node->next)
        result.push_back(assignmentData(node));
    return result;
}
vector<Assignment> AssignmentManager::getPendingAssignments() const {
    vector<Assignment> result;
    for (auto* node = head; node; node = node->next)
        if (assignmentData(node).getStatus() != "Completed" &&
            assignmentData(node).getRemainingMinutes() > 0)
            result.push_back(assignmentData(node));
    return result;
}
