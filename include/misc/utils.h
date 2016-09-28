#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_ 1

#include <stddef.h>

#define deref(p, t) (*((t *) p))


typedef int (*CompareFunction) (const void *, const void *);


int ref_compare(const void *, const void *);

void swap(void *, void *, size_t);


#endif /* ifndef _UTILS_H_ */
