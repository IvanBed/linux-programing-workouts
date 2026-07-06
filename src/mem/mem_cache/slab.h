
#ifndef SLAB_H
#define SLAB_H

#include "block.h"
#define PAGE_SIZE 4096

void *alloc_slab(int );
void free_slab(void *);
bool slab_is_full(slab *);
slab *create_slab(size_t, size_t, size_t);

#endif

