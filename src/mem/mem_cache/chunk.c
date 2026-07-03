#include "chunck.h"

void init_chunk_list(slab *slab, size_t object_size, size_t capacity)
{
    if (!slab)
    {
        return;
    }
    
    chunk  chunk;
    size_t offset;
    chunk *prev_chunk;
    char  *raw_data_ptr;
    
    raw_data_ptr = (char*)slab->raw_data;
    
    offset = object_size + sizeof(struct chunk);
   
    chunk.size = object_size;
    chunk.next_chunk = NULL;
    chunk.slab_ptr = (void*)slab;
    memcpy(raw_data_ptr, (char*)&chunk, sizeof(struct chunk));
    
    slab->first_chunk = (chunk*)raw_data_ptr;
     
    prev_chunk = slab->first_chunk;
   
    size_t index = 1;
    for ( ;index < capacity; index++)
    {
        
        raw_data_ptr += offset;
        chunk.size = object_size;
        chunk.next_chunk = NULL;
        memcpy(raw_data_ptr, (char*)&chunk, sizeof(struct chunk));
        
        prev_chunk->next_chunk = (chunk*)raw_data_ptr;
        prev_chunk = (chunk*)raw_data_ptr;
    }
}

size_t calc_chunk_size(size_t object_size)
{
    size_t chunk_size = sizeof(struct chunk) + object_size;
    return chunk_size
}

void *alloc_chunk(slab *slab)
{
    /*TEST!!!*/
    if (slab->size >= slab->capacity)
        return NULL;
        
    void * chunk      = slab->first_chunk;
    slab->first_chunk = slab->first_chunk->next_chunk;
    slab->size++;
    
    return (void*)((char*)chunk + sizeof(struct chunk));
}

void free_chunk(slab *slab, chunk *target_chunk)
{
    if (slab->size == 0)
        return;
    if (slab->first_chunk == target_chunk)
        return;
        
    target_chunk->next_chunk = slab->first_chunk;
    slab->first_chunk = target_chunk;
    slab->size--;
}

void test_chunks_list(slab *slab)
{
    chunk *chunk = slab->first_chunk;
    size_t index = 0;
    while(chunk)
    {
        chunk = chunk->next_chunk;
        printf("%ld \n",  chunk);
    }
}