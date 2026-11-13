#include "Dsa.h"
#include <stdlib.h>

DsaStatus dsa_list_insert(DsaListNode** link, void* value) {
    if (!link)
        return DSA_INVALID_INPUT;
    DsaListNode* node = malloc(sizeof(*node));
    if (!node)
        return DSA_OUT_OF_MEMORY;
    node->value = value;
    node->sort_key = 0;
    node->next = *link;
    *link = node;
    return DSA_OK;
}

DsaListNode* dsa_list_find(DsaListNode* head, int (*matches)(const void*, const void*),
                           const void* key) {
    if (!matches)
        return NULL;
    for (DsaListNode* node = head; node; node = node->next)
        if (matches(node->value, key))
            return node;
    return NULL;
}

void* dsa_list_remove(DsaListNode** head, DsaListNode* target) {
    if (!head || !target)
        return NULL;
    DsaListNode** link = head;
    while (*link && *link != target)
        link = &(*link)->next;
    if (!*link)
        return NULL;
    void* value = target->value;
    *link = target->next;
    free(target);
    return value;
}

void dsa_list_clear(DsaListNode** head, void (*destroy_value)(void*)) {
    if (!head)
        return;
    while (*head) {
        DsaListNode* node = *head;
        *head = node->next;
        if (destroy_value)
            destroy_value(node->value);
        free(node);
    }
}

static DsaListNode* merge(DsaListNode* left, DsaListNode* right) {
    DsaListNode* result = NULL;
    DsaListNode** tail = &result;
    while (left && right) {
        /* Choosing the left node on a tie preserves input order. */
        DsaListNode** next = left->sort_key <= right->sort_key ? &left : &right;
        *tail = *next;
        *next = (*next)->next;
        tail = &(*tail)->next;
    }
    *tail = left ? left : right;
    return result;
}

void dsa_list_sort(DsaListNode** head) {
    if (!head || !*head || !(*head)->next)
        return;
    DsaListNode* slow = *head;
    DsaListNode* fast = (*head)->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    DsaListNode* right = slow->next;
    slow->next = NULL;
    dsa_list_sort(head);
    dsa_list_sort(&right);
    *head = merge(*head, right);
}
