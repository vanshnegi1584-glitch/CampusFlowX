#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Assignment.h"
#include <vector>

using namespace std;

class Scheduler {
  public:
    struct Recommendation {
        Assignment assignment;
        double score;
        bool isPartial;
        int plannedMinutes;

        Recommendation(const Assignment& a, double s, bool p = false, int minutes = 0)
            : assignment(a), score(s), isPartial(p), plannedMinutes(minutes) {}
    };

    static vector<Recommendation> greedyRecommend(const vector<Assignment>& assignments,
                                                  int availableMinutes);
    static vector<Recommendation> knapsackRecommend(const vector<Assignment>& assignments,
                                                    int availableMinutes);
    static void displayRecommendations(const vector<Recommendation>& recs, int availableMinutes);
};

#endif
