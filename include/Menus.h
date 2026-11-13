#ifndef MENUS_H
#define MENUS_H

#include <string>

class Student;
class Timetable;
class ClassroomFinder;
class AssignmentManager;
class RouteNavigator;

void studentMenu(Student& student, Timetable& timetable, ClassroomFinder& finder,
                 AssignmentManager& assignments, RouteNavigator& navigator,
                 const std::string& assignmentFile);
void teacherMenu(ClassroomFinder& finder);

#endif
