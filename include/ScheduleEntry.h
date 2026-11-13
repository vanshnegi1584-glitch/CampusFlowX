#ifndef SCHEDULE_ENTRY_H
#define SCHEDULE_ENTRY_H

#include <string>
#include <algorithm>

class ScheduleEntry {
  private:
    std::string course;
    int semester;
    std::string section;
    std::string day;
    std::string subject;
    int startTime;
    int endTime;
    std::string roomNumber;

  public:
    ScheduleEntry() : semester(0), startTime(0), endTime(0) {}

    ScheduleEntry(const std::string& c, int sem, const std::string& sec, const std::string& d,
                  const std::string& subj, int start, int end, const std::string& room)
        : course(c), semester(sem), section(sec), day(d), subject(subj), startTime(start),
          endTime(end), roomNumber(room) {}

    const std::string& getCourse() const { return course; }
    int getSemester() const { return semester; }
    const std::string& getSection() const { return section; }
    const std::string& getDay() const { return day; }
    const std::string& getSubject() const { return subject; }
    int getStartTime() const { return startTime; }
    int getEndTime() const { return endTime; }
    const std::string& getRoomNumber() const { return roomNumber; }

    void setCourse(const std::string& c) { course = c; }
    void setSemester(int s) { semester = s; }
    void setSection(const std::string& s) { section = s; }
    void setDay(const std::string& d) { day = d; }
    void setSubject(const std::string& s) { subject = s; }
    void setStartTime(int s) { startTime = s; }
    void setEndTime(int e) { endTime = e; }
    void setRoomNumber(const std::string& r) { roomNumber = r; }

    std::string getKey() const {
        std::string key = course + "-" + std::to_string(semester) + "-" + section;
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        return key;
    }

    bool overlaps(int start, int end) const { return start < endTime && end > startTime; }

    void display(int index = -1) const;
};

#endif