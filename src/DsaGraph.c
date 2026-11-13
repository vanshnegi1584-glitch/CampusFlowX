#include "Dsa.h"
#include <stdlib.h>

typedef struct {
    int64_t distance;
    size_t vertex;
} HeapEntry;

static int comes_before(HeapEntry a, HeapEntry b) {
    if (a.distance != b.distance)
        return a.distance < b.distance;
    return a.vertex < b.vertex;
}

static void heap_push(HeapEntry* heap, size_t* count, HeapEntry entry) {
    size_t index = (*count)++;
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (!comes_before(entry, heap[parent]))
            break;
        heap[index] = heap[parent];
        index = parent;
    }
    heap[index] = entry;
}

static HeapEntry heap_pop(HeapEntry* heap, size_t* count) {
    HeapEntry result = heap[0];
    HeapEntry last = heap[--(*count)];
    size_t index = 0;
    while (index < *count / 2) {
        size_t child = index * 2 + 1;
        if (child + 1 < *count && comes_before(heap[child + 1], heap[child]))
            ++child;
        if (!comes_before(heap[child], last))
            break;
        heap[index] = heap[child];
        index = child;
    }
    if (*count)
        heap[index] = last;
    return result;
}

DsaStatus dsa_dijkstra(size_t vertex_count, const size_t* offsets, const DsaEdge* edges,
                       size_t source, size_t destination, int64_t* distances, size_t* parents) {
    if (!vertex_count || !offsets || !distances || !parents || source >= vertex_count ||
        destination >= vertex_count || offsets[0] != 0)
        return DSA_INVALID_INPUT;
    const size_t edge_count = offsets[vertex_count];
    if (edge_count && !edges)
        return DSA_INVALID_INPUT;
    for (size_t i = 0; i < vertex_count; ++i)
        if (offsets[i] > offsets[i + 1])
            return DSA_INVALID_INPUT;
    for (size_t i = 0; i < edge_count; ++i)
        if (edges[i].destination >= vertex_count || edges[i].minutes <= 0)
            return DSA_INVALID_INPUT;
    if (edge_count >= SIZE_MAX / sizeof(HeapEntry))
        return DSA_LIMIT_EXCEEDED;
    /* Every successful relaxation adds at most one entry per directed edge. */
    HeapEntry* heap = malloc((edge_count + 1) * sizeof(*heap));
    if (!heap)
        return DSA_OUT_OF_MEMORY;
    for (size_t i = 0; i < vertex_count; ++i) {
        distances[i] = INT64_MAX;
        parents[i] = SIZE_MAX;
    }
    distances[source] = 0;
    size_t heap_size = 0;
    heap_push(heap, &heap_size, (HeapEntry){0, source});
    while (heap_size) {
        HeapEntry current = heap_pop(heap, &heap_size);
        if (current.distance != distances[current.vertex])
            continue;
        if (current.vertex == destination)
            break;
        for (size_t i = offsets[current.vertex]; i < offsets[current.vertex + 1]; ++i) {
            DsaEdge edge = edges[i];
            if (current.distance > INT64_MAX - edge.minutes)
                continue;
            int64_t candidate = current.distance + edge.minutes;
            if (candidate < distances[edge.destination]) {
                distances[edge.destination] = candidate;
                parents[edge.destination] = current.vertex;
                heap_push(heap, &heap_size, (HeapEntry){candidate, edge.destination});
            }
        }
    }
    free(heap);
    return DSA_OK;
}
