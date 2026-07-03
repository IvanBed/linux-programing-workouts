#include "slab.h"

void *alloc_slab(int order);

void free_slab(void *slab);

void *alloc_slab(int order)
{
    void *ptr = (void *) malloc(PAGE_SIZE * order);
    return ptr; 
}

void free_slab(void *slab)
{
    free(slab);
}

bool slab_is_full(struct cache *cache, slab *slab)
{
    if (slab->size >= cache->slab_objects)
        return true;
    else 
        return false;

}

slab *create_slab(size_t object_size, size_t slab_order, size_t slab_objects)
{
    
    slab *slab       = (struct slab*)alloc_slab(slab_order);
   
    slab->size       = 0;
    slab->capacity   = slab_objects;
    
    slab->prev_slab  = NULL;
    slab->next_slab  = NULL;
    slab->chunk_size = object_size;
    slab->raw_data   = (void*)((char*)slab + sizeof(struct slab));
    
    init_chunk_list(slab, object_size, slab_objects);
    return slab;
}


void test_slabs_list(slab_list *list)
{
    slab *slab = list->head;
    while(slab)
    {
        printf("%ld \n",  slab);
        slab = slab->prev_slab;
    }
}