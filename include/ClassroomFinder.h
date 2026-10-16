#ifndef CLASSROOM_FINDER_H
#define CLASSROOM_FINDER_H
#include "Timetable.h"
#include <string>

using namespace std;


class ClassroomFinder {
private:
    const Timetable& timetable;
public:
    explicit ClassroomFinder(const Timetable& timetable);
    // Unknown rooms and invalid requests are reported, never called available.
    void checkRoomAvailability(const string& room, const string& day,
                               int time) const;
    void displayRooms() const;
};
#endif
