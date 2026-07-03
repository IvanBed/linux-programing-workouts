#include "chunk.h"
#define PAGE_SIZE 4096

typedef struct slab 
{
    size_t size;
    size_t capacity;
    size_t chunk_size;
    chunk *first_chunk;
    struct slab *prev_slab; 
    struct slab *next_slab; 
    void  *raw_data;
    void  *list_ptr;
} slab;
