#pragma once

#ifndef _HEAP_H_
#define _HEAP_H_ 1

#include<sys/types.h>
#include<stdbool.h>
#include<stdint.h>

#include "misc/utils.h"

typedef struct Heap Heap;


Heap *heap_new(CompareFunction);
void heap_destroy(Heap *);
void heap_push(Heap *, void *);
void *heap_pop(Heap *);
void *heap_peek(const Heap *);
void heap_update(Heap *, void *, void (*) (void **, void *), void *);
void heap_updateIf(Heap *, bool (*) (const void *, void *), void *, void (*) (void **, void *), void *);

bool heap_isEmpty(const Heap *);
size_t heap_getSize(const Heap *);

#endif /* ifndef _HEAP_H_ */
