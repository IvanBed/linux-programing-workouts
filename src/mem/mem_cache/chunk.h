#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct chunk 
{
    size_t size;
    struct chunk *next_chunk;
    void  *slab_ptr;
} chunk;