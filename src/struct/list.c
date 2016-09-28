#include <stdlib.h>

#include "struct/list.h"

#define headOf(n, l) (n == l->head)

typedef struct ListNode {
    void *value;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

struct List {
    ListNode *head;
    size_t length;
};

struct ListIterator {
    List *parent;
    ListNode *current;
};

ListNode *listnode_new(ListNode *prev, void *value) {
    ListNode *n = (ListNode *) malloc(sizeof(*n));

    n->prev = prev;
    n->next = prev->next;
    n->value = value;
    prev->next->prev = n;
    prev->next = n;

    return n;
}

void *_node_remove(ListNode *n) {
    void *val = n->value;
    n->prev->next = n->next;
    n->next->prev = n->prev;
    free(n);
    return val;
}

List *list_new(void) {
    List *l = (List *) malloc(sizeof(List));

    l->head = (ListNode *) malloc(sizeof(*l->head));
    l->head->next = l->head->prev = l->head;
    l->head->value = NULL;
    l->length = 0;

    return l;
}

void list_destroy(List *l) {
    ListNode *curr = l->head->next,
         *next;
    while (!headOf(curr, l)) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(curr);
    free(l);
}

void list_append(List *l, void *value) {
    listnode_new(l->head->prev, value);
    l->length++;
}

void *list_getFirst(const List *l) {
    return l->head->next->value;
}

void *list_getLast(const List *l) {
    return l->head->prev->value;
}

void *list_removeFirst(List *l) {
    void *val;
    ListIterator *iter = list_iterator(l);
    (void) listiter_next(iter);
    val = listiter_remove(iter);
    listiter_destroy(iter);
    return val;
}

void *list_removeLast(List *l) {
    void *val;
    ListIterator *iter = list_iterator(l);
    (void) listiter_prev(iter);
    val = listiter_remove(iter);
    listiter_destroy(iter);
    return val;
}

bool list_contains(const List *l, void *value) {
    ListNode *curr = l->head->next;

    while (!headOf(curr, l)) {
        if (curr->value == value)
            return true;
        curr = curr->next;
    }

    return false;
}

bool list_containsCmp(const List *l, void *value, CompareFunction cmp) {
    ListNode *curr = l->head->next;

    while (!headOf(curr, l)) {
        if (cmp(curr->value, value) == 0)
            return true;
        curr = curr->next;
    }

    return false;
}

void *list_remove(List *l, void *value) {
    ListNode *curr = l->head->next;

    while (!headOf(curr, l)) {
        if (curr->value == value) {
            l->length--;
            return _node_remove(curr);
        }
        curr = curr->next;
    }

    return NULL;
}

void *list_removeCmp(List *l, void *value, int (*compare) (const void *, const void *)) {
    ListNode *curr = l->head->next;

    while (!headOf(curr, l)) {
        if (!compare(curr->value, value)) {
            l->length--;
            return _node_remove(curr);
        }
        curr = curr->next;
    }

    return NULL;
}

size_t list_length(const List *l) {
    return l->length;
}

ListIterator *list_iterator(List *l) {
    ListIterator *iter = (ListIterator *) malloc(sizeof(ListIterator));

    iter->parent = l;
    iter->current = l->head;

    return iter;
}

ListIterator *listiter_copy(const ListIterator *original) {
    ListIterator *iter = (ListIterator *) malloc(sizeof(ListIterator));

    iter->parent = original->parent;
    iter->current = original->current;

    return iter;
}

bool listiter_hasPrev(const ListIterator *iter) {
    return !headOf(iter->current->prev, iter->parent);
}

bool listiter_hasCurr(const ListIterator *iter) {
    return !headOf(iter->current, iter->parent);
}

bool listiter_hasNext(const ListIterator *iter) {
    return !headOf(iter->current->next, iter->parent);
}

void *listiter_prev(ListIterator *iter) {
    iter->current = iter->current->prev;
    return iter->current->value;
}

void *listiter_curr(const ListIterator *iter) {
    return iter->current->value;
}

void *listiter_next(ListIterator *iter) {
    iter->current = iter->current->next;
    return iter->current->value;
}

void *listiter_remove(ListIterator *iter) {
    ListNode *curr = iter->current;

    if (listiter_hasCurr(iter)) {
        iter->parent->length--;
        iter->current = iter->current->next;
        return _node_remove(curr);
    }
    return NULL;
}

void listiter_destroy(ListIterator *iter) {
    free(iter);
}
