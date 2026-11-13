#include "ClassroomFinder.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace {
bool naturalRoomSort(const std::string& a, const std::string& b) {
    auto prefix = [](const std::string& room) {
        auto end = room.find_first_not_of("0123456789");
        std::string num = room.substr(0, end);
        auto nonzero = num.find_first_not_of('0');
        return nonzero == std::string::npos ? std::string() : num.substr(nonzero);
    };
    auto x = prefix(a), y = prefix(b);
    if (x.size() != y.size())
        return x.size() < y.size();
    if (x != y)
        return x < y;
    return a < b;
}
bool validDay(const std::string& day) {
    if (DateTimeUtils::isValidDay(day))
        return true;
    std::cout << "Unknown weekday.\n";
    return false;
}
} // namespace
std::vector<std::string> ClassroomFinder::findVacantRooms(const std::string& day, int startTime,
                                                          int endTime) const {
    std::vector<std::string> vacant;
    if (!DateTimeUtils::isValidDay(day) || startTime < DateTimeUtils::COLLEGE_START_MINUTES ||
        endTime > DateTimeUtils::COLLEGE_END_MINUTES || startTime >= endTime)
        return vacant;
    for (const auto& room : timetable.getAllRooms()) {
        const auto entries = timetable.getEntriesForRoom(room, day);
        if (std::none_of(entries.begin(), entries.end(),
                         [&](const auto& e) { return e.overlaps(startTime, endTime); }))
            vacant.push_back(room);
    }
    std::sort(vacant.begin(), vacant.end(), naturalRoomSort);
    return vacant;
}
void ClassroomFinder::displayVacantRooms(const std::vector<std::string>& rooms,
                                         const std::string& day, int startTime, int endTime) const {
    std::cout << "\nVacant classrooms | " << DateTimeUtils::normalizeDay(day) << " | "
              << DateTimeUtils::minutesToTime(startTime) << " - "
              << DateTimeUtils::minutesToTime(endTime) << '\n';
    if (rooms.empty())
        std::cout << "No vacant classrooms for this interval.\n";
    else
        for (size_t i = 0; i < rooms.size(); ++i)
            std::cout << i + 1 << ". " << rooms[i] << '\n';
    std::cout << "Availability is based on the weekly timetable. Every room listed in a combined "
                 "entry is treated as occupied.\n";
}
void ClassroomFinder::checkRoomAvailability(const std::string& room, const std::string& day) const {
    if (!validDay(day))
        return;
    const auto key = timetable.resolveRoom(room);
    const auto& rooms = timetable.getAllRooms();
    if (std::find(rooms.begin(), rooms.end(), key) == rooms.end()) {
        std::cout << "Room not found in timetable data.\n";
        return;
    }
    auto entries = timetable.getEntriesForRoom(key, day);
    std::vector<std::pair<int, int>> occupied;
    for (const auto& e : entries) {
        if (occupied.empty() || e.getStartTime() > occupied.back().second)
            occupied.emplace_back(e.getStartTime(), e.getEndTime());
        else
            occupied.back().second = std::max(occupied.back().second, e.getEndTime());
    }
    std::cout << "\nRoom " << key << " | " << DateTimeUtils::normalizeDay(day)
              << "\nOccupied periods:\n";
    for (const auto& period : occupied)
        std::cout << "  " << DateTimeUtils::minutesToTime(period.first) << " - "
                  << DateTimeUtils::minutesToTime(period.second) << '\n';
    if (occupied.empty())
        std::cout << "  None\n";
    std::cout << "Free periods:\n";
    int last = DateTimeUtils::COLLEGE_START_MINUTES;
    bool found = false;
    auto show = [&](int end) {
        std::cout << "  " << DateTimeUtils::minutesToTime(last) << " - "
                  << DateTimeUtils::minutesToTime(end) << " (" << end - last << " min)\n";
        found = true;
    };
    for (const auto& period : occupied) {
        if (period.first > last)
            show(period.first);
        last = std::max(last, period.second);
    }
    if (last < DateTimeUtils::COLLEGE_END_MINUTES)
        show(DateTimeUtils::COLLEGE_END_MINUTES);
    if (!found)
        std::cout << "  None during college hours\n";
}
void ClassroomFinder::displayRoomSchedule(const std::string& room, const std::string& day) const {
    if (!validDay(day))
        return;
    const auto key = timetable.resolveRoom(room);
    const auto& rooms = timetable.getAllRooms();
    if (std::find(rooms.begin(), rooms.end(), key) == rooms.end()) {
        std::cout << "Room not found in timetable data.\n";
        return;
    }
    auto entries = timetable.getEntriesForRoom(key, day);
    std::cout << "\nRoom schedule: " << key << " | " << DateTimeUtils::normalizeDay(day) << '\n';
    if (entries.empty())
        std::cout << "No classes scheduled in this room on this day.\n";
    for (const auto& e : entries) {
        std::cout << DateTimeUtils::minutesToTime(e.getStartTime()) << " - "
                  << DateTimeUtils::minutesToTime(e.getEndTime()) << " | " << e.getSubject()
                  << "\n  " << e.getCourse() << " | Semester " << e.getSemester() << " | Section "
                  << e.getSection() << '\n';
        if (DateTimeUtils::splitRooms(e.getRoomNumber()).size() > 1)
            std::cout << "  Listed rooms: " << e.getRoomNumber() << '\n';
    }
}
