#include "slab.h"

//buddy allocator will be added soon
void *alloc_slab(int order)
{
    void *ptr = (void *) malloc(PAGE_SIZE * order);
    return ptr; 
}

void free_slab(void *slab)
{
    free(slab);
}

bool slab_is_full(slab *slab)
{
    if (slab == NULL)
    {
        printf("Slab pointer is null\n");
        return;
    }
    
    if (slab->size >= slab->capacity)
        return true;
    else
        return false;
}

slab *create_slab(size_t object_size, size_t slab_order, size_t slab_objects)
{
    slab *slab        = (struct slab*)alloc_slab(slab_order);
    if (!slab)
    {
        printf("Can not allocate memory chunk for slab\n");
        return NULL;
    }

    slab->size        = 0;
    slab->capacity    = slab_objects;

    slab->prev_slab   = NULL;
    slab->next_slab   = NULL;
    slab->block_size   = object_size;
    slab->raw_data    = (void*)((char*)slab + sizeof(struct slab));
    slab->list_ptr    = NULL;

    init_block_list(slab, object_size, slab_objects);
    return slab;
}

