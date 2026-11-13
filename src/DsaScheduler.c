#include "Dsa.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    DsaTask task;
    size_t index;
} RankedTask;

static int rank_task(const void* first, const void* second) {
    const RankedTask* a = first;
    const RankedTask* b = second;
    if (a->task.score != b->task.score)
        return a->task.score > b->task.score ? -1 : 1;
    if (a->task.id != b->task.id)
        return a->task.id < b->task.id ? -1 : 1;
    return (a->index > b->index) - (a->index < b->index);
}

static int valid_tasks(const DsaTask* tasks, size_t count, DsaSelection* selections) {
    if (count && (!tasks || !selections))
        return 0;
    for (size_t i = 0; i < count; ++i)
        if (tasks[i].minutes <= 0 || !isfinite(tasks[i].score))
            return 0;
    return 1;
}

DsaStatus dsa_greedy(const DsaTask* tasks, size_t count, int available_minutes,
                     DsaSelection* selections, size_t* selection_count) {
    if (!selection_count)
        return DSA_INVALID_INPUT;
    *selection_count = 0;
    if (!valid_tasks(tasks, count, selections))
        return DSA_INVALID_INPUT;
    if (!count || available_minutes <= 0)
        return DSA_OK;
    if (count > SIZE_MAX / sizeof(RankedTask))
        return DSA_LIMIT_EXCEEDED;
    RankedTask* ranked = malloc(count * sizeof(*ranked));
    if (!ranked)
        return DSA_OUT_OF_MEMORY;
    for (size_t i = 0; i < count; ++i) {
        ranked[i].task = tasks[i];
        ranked[i].index = i;
    }
    qsort(ranked, count, sizeof(*ranked), rank_task);
    int remaining = available_minutes;
    for (size_t i = 0; i < count; ++i) {
        int minutes = ranked[i].task.minutes;
        if (minutes <= remaining) {
            selections[(*selection_count)++] = (DsaSelection){ranked[i].index, minutes, 0};
            remaining -= minutes;
        }
    }
    if (!*selection_count) {
        selections[0] = (DsaSelection){ranked[0].index, available_minutes, 1};
        *selection_count = 1;
    }
    free(ranked);
    return DSA_OK;
}

DsaStatus dsa_knapsack(const DsaTask* tasks, size_t count, int available_minutes,
                       DsaSelection* selections, size_t* selection_count) {
    if (!selection_count)
        return DSA_INVALID_INPUT;
    *selection_count = 0;
    if (!valid_tasks(tasks, count, selections))
        return DSA_INVALID_INPUT;
    if (!count || available_minutes <= 0)
        return DSA_OK;

    /* Cap total work at the requested time without overflowing. */
    int capacity = 0;
    size_t fitting_count = 0;
    for (size_t i = 0; i < count; ++i) {
        if (tasks[i].minutes > available_minutes)
            continue;
        ++fitting_count;
        int room = available_minutes - capacity;
        capacity += tasks[i].minutes < room ? tasks[i].minutes : room;
    }
    if (!fitting_count)
        return DSA_OK;
    size_t columns = (size_t)capacity + 1;
    if (capacity > 10000 || fitting_count > 20000000u / columns)
        return DSA_LIMIT_EXCEEDED;

    double* best = calloc(columns, sizeof(*best));
    size_t bytes_per_row = (columns + 7) / 8;
    unsigned char* selected = calloc(fitting_count, bytes_per_row);
    if (!best || !selected) {
        free(best);
        free(selected);
        return DSA_OUT_OF_MEMORY;
    }

    size_t row = 0;
    for (size_t i = 0; i < count; ++i) {
        if (tasks[i].minutes > available_minutes)
            continue;
        for (int time = capacity; time >= tasks[i].minutes; --time) {
            double candidate = best[time - tasks[i].minutes] + tasks[i].score;
            if (candidate > best[time]) {
                best[time] = candidate;
                selected[row * bytes_per_row + (size_t)time / 8] |=
                    (unsigned char)(1u << (time % 8));
            }
        }
        ++row;
    }

    int remaining = capacity;
    for (size_t i = count; i > 0; --i) {
        const DsaTask* task = &tasks[i - 1];
        if (task->minutes > available_minutes)
            continue;
        --row;
        if (selected[row * bytes_per_row + (size_t)remaining / 8] & (1u << (remaining % 8))) {
            selections[(*selection_count)++] = (DsaSelection){i - 1, task->minutes, 0};
            remaining -= task->minutes;
        }
    }
    for (size_t i = 0; i < *selection_count / 2; ++i) {
        DsaSelection temp = selections[i];
        selections[i] = selections[*selection_count - 1 - i];
        selections[*selection_count - 1 - i] = temp;
    }
    free(best);
    free(selected);
    return DSA_OK;
}
