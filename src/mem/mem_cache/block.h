#ifndef BLOCK_H
#define BLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct block 
{
    size_t size;
    struct block *next_block;
    void  *slab_ptr;
} block;

typedef struct slab 
{
    size_t size;
    size_t capacity;
    size_t block_size;
    block *first_block;
    struct slab *prev_slab;
    struct slab *next_slab;
    void  *list_ptr;
    void  *raw_data;
} slab;

void *alloc_block(slab *);
void free_block(slab *, block *);
void init_block_list(slab *, size_t, size_t);

#endif