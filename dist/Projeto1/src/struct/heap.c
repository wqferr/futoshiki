#include <stdlib.h>
#include <limits.h>
#include <stddef.h>

#include "struct/heap.h"
#include "misc/utils.h"

#define getChild(n, r) ((r) ? n->rchild : n->lchild)
#define setChild(p, n, r) *((r) ? &p->rchild : &p->lchild) = n
#define nBits(n) (CHAR_BIT*sizeof(n))
#define swapVals(u, v) (swap(&u->value, &v->value, sizeof(u->value)))

typedef struct HeapNode {
    struct HeapNode *lchild;
    struct HeapNode *rchild;

    void *value;
} HeapNode;

struct Heap {
    HeapNode *root;
    size_t size;

    CompareFunction cmp;
};

HeapNode *heapnode_new(void *v) {
    HeapNode *node = (HeapNode *) malloc(sizeof(*node));

    node->lchild = NULL;
    node->rchild = NULL;

    node->value = v;

    return node;
}

uint8_t mostSignificantBit(size_t n) {
    uint8_t b = 0;
    while (!(n >> (nBits(n) - 1 - b)))
        b++;
    return b;
}

void _heap_fix(HeapNode *node, CompareFunction cmp) {
    HeapNode *smallestChild = node;

    if (node->lchild != NULL && cmp(node->lchild->value, smallestChild->value) < 0)
        smallestChild = node->lchild;
    if (node->rchild != NULL && cmp(node->rchild->value, smallestChild->value) < 0)
        smallestChild = node->rchild;

    if (smallestChild != node) {
        swapVals(node, smallestChild);
        _heap_fix(smallestChild, cmp);
    }
}

HeapNode *_heap_push(HeapNode *node, size_t path, uint8_t step, void *v, CompareFunction cmp) {
    bool childDir;
    HeapNode *child;

    if (step == nBits(path))
        return heapnode_new(v);

    childDir = ((path >> (nBits(path) - 1 - step)) & 1);
    setChild(node, _heap_push(getChild(node, childDir), path, step + 1, v, cmp), childDir);
    child = getChild(node, childDir);

    if (cmp(child->value, node->value) < 0)
        swapVals(node, child);

    return node;
}

bool _heap_pop(HeapNode *node, size_t path, uint8_t step, CompareFunction cmp) {
    bool childDir;
    HeapNode *child;

    if (step < nBits(path)) {
        childDir = ((path >> (nBits(path) - 1 - step)) & 1);
        child = getChild(node, childDir);
        swapVals(node, child);

        if (!_heap_pop(child, path, step + 1, cmp)) {
            setChild(node, child->lchild, childDir);
            free(child);
        }

        _heap_fix(node, cmp);
        return true;
    }

    return false;
}



Heap *heap_new(CompareFunction cmp) {
    Heap *h = (Heap *) malloc(sizeof(*h));

    h->root = NULL;
    h->size = 0;
    h->cmp = cmp;

    return h;
}

void _heap_destroy(HeapNode *node) {
    if (node != NULL) {
        _heap_destroy(node->lchild);
        _heap_destroy(node->rchild);
        free(node);
    }
}

void heap_destroy(Heap *h) {
    _heap_destroy(h->root);
    free(h);
}

void heap_push(Heap *h, void *v) {
    h->size++;
    uint8_t step = mostSignificantBit(h->size);
    h->root = _heap_push(h->root, h->size, step + 1, v, h->cmp);
}

void *heap_pop(Heap *h) {
    uint8_t step;
    void *v;
    if (heap_isEmpty(h))
        return NULL;

    step = mostSignificantBit(h->size);
    v = h->root->value;

    _heap_pop(h->root, h->size, step + 1, h->cmp);
    h->size--;

    return v;
}

void *heap_peek(const Heap *h) {
    if (heap_isEmpty(h))
        return NULL;
    return h->root->value;
}

bool _heap_update(HeapNode *n, void *key, void (*process) (void **, void *), void *arg, CompareFunction cmp) {
    int c;
    bool success;

    if (n != NULL) {
        c = cmp(n->value, key);
        if (c <= 0) {
            success = _heap_update(n->lchild, key, process, arg, cmp);
            success = _heap_update(n->rchild, key, process, arg, cmp) || success;
            success = success || c == 0;

            if (c == 0)
                process(&(n->value), arg);

            if (success) {
                _heap_fix(n, cmp);
                return true;
            }
        }
    }

    return false;
}

void heap_update(Heap *h, void *key, void (*process) (void **, void *), void *arg) {
    _heap_update(h->root, key, process, arg, h->cmp);
}

bool _heap_updateIf(HeapNode *n, bool (*check) (const void *, void *), void *arg1, void (*process) (void **, void *), void *arg2, CompareFunction cmp) {
    bool success;
    if (n != NULL) {
        success = false;
        if (check(n->value, arg1)) {
            process(&(n->value), arg2);
            success = true;
        }

        success = _heap_updateIf(n->lchild, check, arg1, process, arg2, cmp) || success;
        success = _heap_updateIf(n->rchild, check, arg1, process, arg2, cmp) || success;

        if (success) {
            _heap_fix(n, cmp);
            return true;
        }
    }
    return false;
}

void heap_updateIf(Heap *h, bool (*check) (const void *, void *), void *arg1, void (*process) (void **, void *), void *arg2) {
    _heap_updateIf(h->root, check, arg1, process, arg2, h->cmp);
}

bool heap_isEmpty(const Heap *h) {
    return h->size == 0;
}

size_t heap_getSize(const Heap *h) {
    return h->size;
}
