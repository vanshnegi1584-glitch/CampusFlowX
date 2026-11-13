#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <string>
#include <algorithm>

using namespace std;

class Assignment {
  private:
    int id;
    string title;
    string subject;
    string deadline;
    int estimatedDuration;
    string priority;
    string status;
    int completedMinutes;

  public:
    Assignment() : id(0), estimatedDuration(0), completedMinutes(0) {}

    Assignment(int i, const string& t, const string& subj, const string& dl, int dur,
               const string& pri, const string& stat, int comp = 0)
        : id(i), title(t), subject(subj), deadline(dl), estimatedDuration(dur), priority(pri),
          status(stat), completedMinutes(comp) {}

    int getId() const { return id; }
    const string& getTitle() const { return title; }
    const string& getSubject() const { return subject; }
    const string& getDeadline() const { return deadline; }
    int getEstimatedDuration() const { return estimatedDuration; }
    const string& getPriority() const { return priority; }
    const string& getStatus() const { return status; }
    int getCompletedMinutes() const { return completedMinutes; }
    int getRemainingMinutes() const {
        return status == "Completed" ? 0 : std::max(0, estimatedDuration - completedMinutes);
    }

    void setId(int i) { id = i; }
    void setTitle(const string& t) { title = t; }
    void setSubject(const string& s) { subject = s; }
    void setDeadline(const string& d) { deadline = d; }
    void setEstimatedDuration(int d) { estimatedDuration = d; }
    void setPriority(const string& p) { priority = p; }
    void setStatus(const string& s) { status = s; }
    void setCompletedMinutes(int c) { completedMinutes = c; }

    int getPriorityWeight() const {
        if (priority == "High")
            return 3;
        if (priority == "Medium")
            return 2;
        return 1;
    }

    bool isOverdue() const;
    bool isValid() const;
    void reconcileProgress();
    int getDaysUntilDeadline() const;
    double calculateScore(int availableMinutes) const;

    void display(int index = -1) const;
    string toCSV() const;
    static Assignment fromCSV(const string& line);
};

#endif
