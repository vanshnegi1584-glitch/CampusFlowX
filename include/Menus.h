#ifndef MENUS_H
#define MENUS_H
#include "Timetable.h"
#include "ClassroomFinder.h"
#include "AssignmentManager.h"
#include "RouteNavigator.h"

void timetableMenu(const Timetable& timetable);
void classroomMenu(const ClassroomFinder& finder);
void assignmentMenu(AssignmentManager& assignments);
void routeMenu(const RouteNavigator& navigator);
#endif
