#ifndef CAMPUS_DSA_H
#define CAMPUS_DSA_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { DSA_OK, DSA_INVALID_INPUT, DSA_OUT_OF_MEMORY, DSA_LIMIT_EXCEEDED } DsaStatus;

/* The list owns its nodes. Values are caller-managed: remove returns a value,
   and clear invokes the caller's destroy callback for every remaining value. */
typedef struct DsaListNode {
    void* value;
    int64_t sort_key;
    struct DsaListNode* next;
} DsaListNode;

/* Insert at a link: &head for the front, &last->next for the tail. */
DsaStatus dsa_list_insert(DsaListNode** link, void* value);
DsaListNode* dsa_list_find(DsaListNode* head, int (*matches)(const void*, const void*),
                           const void* key);
/* Remove the node, returning its value to the caller for disposal. */
void* dsa_list_remove(DsaListNode** head, DsaListNode* target);
void dsa_list_clear(DsaListNode** head, void (*destroy_value)(void*));
/* Stable merge sort; set each node's sort_key before calling. */
void dsa_list_sort(DsaListNode** head);

typedef struct {
    int id;
    int minutes;
    double score;
} DsaTask;

typedef struct {
    size_t task_index;
    int minutes;
    int partial;
} DsaSelection;

/* Caller supplies space for count selections. Inputs require positive minutes
   and finite scores. Equal scores use ID, then input order, as tie breakers. */
DsaStatus dsa_greedy(const DsaTask* tasks, size_t count, int available_minutes,
                     DsaSelection* selections, size_t* selection_count);
/* Exact 0/1 knapsack. Returns LIMIT_EXCEEDED when the work/memory budget is too
   large, allowing the application to explain and choose its fallback. */
DsaStatus dsa_knapsack(const DsaTask* tasks, size_t count, int available_minutes,
                       DsaSelection* selections, size_t* selection_count);

typedef struct {
    size_t destination;
    int minutes;
} DsaEdge;

/* Adjacency lists use offsets[vertex]..offsets[vertex + 1] into edges.
   offsets has vertex_count + 1 entries; distances/parents have vertex_count.
   Vertex IDs determine ties, so the caller assigns IDs in location-name order.
   Unreachable distances are INT64_MAX and missing parents are SIZE_MAX. */
DsaStatus dsa_dijkstra(size_t vertex_count, const size_t* offsets, const DsaEdge* edges,
                       size_t source, size_t destination, int64_t* distances, size_t* parents);

#ifdef __cplusplus
}
#endif

#endif
