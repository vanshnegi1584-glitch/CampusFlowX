#ifndef CLASSROOM_FINDER_H
#define CLASSROOM_FINDER_H

#include "Timetable.h"
#include <vector>
#include <string>

class ClassroomFinder {
  private:
    const Timetable& timetable;

  public:
    ClassroomFinder(const Timetable& tt) : timetable(tt) {}

    std::vector<std::string> findVacantRooms(const std::string& day, int startTime,
                                             int endTime) const;
    void displayVacantRooms(const std::vector<std::string>& rooms, const std::string& day,
                            int startTime, int endTime) const;
    void checkRoomAvailability(const std::string& room, const std::string& day) const;
    void displayRoomSchedule(const std::string& room, const std::string& day) const;
};

#endif