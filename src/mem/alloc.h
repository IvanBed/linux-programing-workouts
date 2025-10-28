#ifndef ALLOC_H
#define ALLOC_H

#include <unistd.h>

void mysetup(void *, size_t);

void * myalloc(size_t);

void myfree(void *);

#endif
