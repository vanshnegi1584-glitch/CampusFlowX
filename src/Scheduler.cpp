#include "Scheduler.h"
#include "Dsa.h"
#include <algorithm>
#include <iostream>
#include <new>
#include <stdexcept>

namespace {
vector<Scheduler::Recommendation> recommend(const vector<Assignment>& assignments,
                                            int availableMinutes, bool optimize) {
    vector<Scheduler::Recommendation> result;
    if (availableMinutes <= 0)
        return result;

    // C++ validates domain objects; the C algorithm receives numeric task data.
    vector<const Assignment*> eligible;
    vector<DsaTask> tasks;
    for (const auto& assignment : assignments) {
        if (!assignment.isValid() || assignment.getStatus() == "Completed" ||
            assignment.getRemainingMinutes() <= 0)
            continue;
        if (optimize && assignment.getRemainingMinutes() > availableMinutes)
            continue;
        eligible.push_back(&assignment);
        tasks.push_back({assignment.getId(), assignment.getRemainingMinutes(),
                         assignment.calculateScore(availableMinutes)});
    }
    vector<DsaSelection> selected(tasks.size());
    size_t selectedCount = 0;
    DsaStatus status = optimize ? dsa_knapsack(tasks.data(), tasks.size(), availableMinutes,
                                               selected.data(), &selectedCount)
                                : dsa_greedy(tasks.data(), tasks.size(), availableMinutes,
                                             selected.data(), &selectedCount);
    if (optimize && status == DSA_LIMIT_EXCEEDED) {
        cout << "This task list is too large for exact optimization; using the quick "
                "recommendation.\n";
        return recommend(assignments, availableMinutes, false);
    }
    if (status == DSA_OUT_OF_MEMORY)
        throw bad_alloc();
    if (status != DSA_OK)
        throw runtime_error("Could not calculate assignment recommendations.");
    for (size_t i = 0; i < selectedCount; ++i) {
        const auto& choice = selected[i];
        result.emplace_back(*eligible[choice.task_index], tasks[choice.task_index].score,
                            choice.partial != 0, choice.minutes);
    }
    return result;
}
} // namespace

vector<Scheduler::Recommendation> Scheduler::greedyRecommend(const vector<Assignment>& assignments,
                                                             int availableMinutes) {
    return recommend(assignments, availableMinutes, false);
}

vector<Scheduler::Recommendation>
Scheduler::knapsackRecommend(const vector<Assignment>& assignments, int availableMinutes) {
    return recommend(assignments, availableMinutes, true);
}

void Scheduler::displayRecommendations(const vector<Recommendation>& recs, int availableMinutes) {
    if (recs.empty()) {
        cout << "\nNo complete tasks fit this period. Try the quick mode for partial work.\n";
        return;
    }
    cout << "\nAssignment recommendations | " << availableMinutes << " minutes available\n";
    int used = 0;
    for (const auto& r : recs) {
        const auto& a = r.assignment;
        int planned = r.plannedMinutes > 0 ? r.plannedMinutes : a.getRemainingMinutes();
        planned = std::clamp(planned, 0, std::max(0, availableMinutes - used));
        used += planned;
        cout << "\nID " << a.getId() << " | " << a.getTitle() << '\n'
             << "  " << a.getSubject() << " | " << a.getPriority() << " priority\n"
             << "  Work for " << planned << " minutes";
        if (r.isPartial)
            cout << " (partial; " << std::max(0, a.getRemainingMinutes() - planned)
                 << " minutes still needed)";
        cout << "\n  Due: " << a.getDeadline() << '\n';
    }
    cout << "\nTotal time used: " << used
         << " minutes\nBuffer time: " << std::max(0, availableMinutes - used) << " minutes\n";
}
