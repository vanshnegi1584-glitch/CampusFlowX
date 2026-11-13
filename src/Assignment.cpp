#include "Assignment.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <climits>
#include <stdexcept>

bool Assignment::isValid() const {
    return id > 0 && id < INT_MAX && !DateTimeUtils::trim(title).empty() &&
           !DateTimeUtils::trim(subject).empty() && DateTimeUtils::isValidDeadline(deadline) &&
           estimatedDuration > 0 && estimatedDuration <= 10000 && completedMinutes >= 0 &&
           completedMinutes <= estimatedDuration &&
           (priority == "High" || priority == "Medium" || priority == "Low") &&
           (status == "Pending" || status == "In Progress" || status == "Completed") &&
           title.find_first_of("\r\n") == string::npos &&
           subject.find_first_of("\r\n") == string::npos;
}
void Assignment::reconcileProgress() {
    completedMinutes = std::clamp(completedMinutes, 0, std::max(0, estimatedDuration));
    if (status == "Completed")
        completedMinutes = estimatedDuration;
    else if (completedMinutes == estimatedDuration && estimatedDuration > 0)
        status = "Completed";
    else if (completedMinutes > 0 && status == "Pending")
        status = "In Progress";
}
bool Assignment::isOverdue() const {
    return status != "Completed" && DateTimeUtils::isValidDeadline(deadline) &&
           DateTimeUtils::parseDeadlineToMinutes(deadline) < 0;
}
int Assignment::getDaysUntilDeadline() const {
    if (!DateTimeUtils::isValidDeadline(deadline))
        return -1;
    int minutes = DateTimeUtils::parseDeadlineToMinutes(deadline);
    return minutes < 0 ? -1 : (minutes + 1439) / 1440;
}
double Assignment::calculateScore(int availableMinutes) const {
    if (status == "Completed" || getRemainingMinutes() <= 0 ||
        !DateTimeUtils::isValidDeadline(deadline))
        return 0;
    int days = getDaysUntilDeadline();
    double urgency = days < 0 ? 100 : days <= 1 ? 50 : days <= 3 ? 30 : days <= 7 ? 15 : 5;
    int remaining = getRemainingMinutes();
    double fit = remaining <= availableMinutes ? (remaining <= availableMinutes / 2 ? 30 : 20) : 0;
    return getPriorityWeight() * 50 + urgency + fit;
}
void Assignment::display(int index) const {
    (void)index;
    cout << "\nID " << id << " | " << title << '\n'
         << "  Subject: " << subject << '\n'
         << "  Due: " << deadline << (isOverdue() ? " [OVERDUE]" : "") << '\n'
         << "  " << priority << " priority | " << status << " | " << completedMinutes << '/'
         << estimatedDuration << " min done (" << getRemainingMinutes() << " min remaining)\n";
}
string Assignment::toCSV() const {
    using DateTimeUtils::escapeCSVField;
    if (!isValid())
        throw invalid_argument("Invalid assignment");
    return to_string(id) + "," + escapeCSVField(title) + "," + escapeCSVField(subject) + "," +
           DateTimeUtils::formatDeadline(deadline) + "," + to_string(estimatedDuration) + "," +
           priority + "," + status + "," + to_string(completedMinutes);
}
Assignment Assignment::fromCSV(const string& line) {
    using namespace DateTimeUtils;
    auto f = parseCSVLine(line);
    int id, duration, completed;
    if (f.size() != 8 || !parseInteger(f[0], id) || !parseInteger(f[4], duration) ||
        !parseInteger(f[7], completed))
        throw invalid_argument("Assignment needs eight fields and valid integers");
    string priority, status;
    for (const auto& p : {"High", "Medium", "Low"})
        if (equalsIgnoreCase(f[5], p))
            priority = p;
    for (const auto& s : {"Pending", "In Progress", "Completed"})
        if (equalsIgnoreCase(f[6], s))
            status = s;
    Assignment a(id, f[1], f[2], formatDeadline(f[3]), duration, priority, status, completed);
    if (!a.isValid())
        throw invalid_argument("Invalid assignment fields or progress");
    a.reconcileProgress();
    return a;
}
