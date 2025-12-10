#ifndef ALLOC_H
#define ALLOC_H

#include <unistd.h>

void mysetup(void *, size_t);

void * myalloc(size_t);

void * mycalloc(size_t, size_t);

void * myrealloc(void *, size_t);

void myfree(void *);

#endif
