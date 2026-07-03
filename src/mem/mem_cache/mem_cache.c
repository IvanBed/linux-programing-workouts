#include "mem_cache.h"
#define SLAB_CHUNKS 5 

size_t calculate_slab_size(size_t slab_order)
{
    size_t base;
    size_t exp;
    size_t slab_size;

    base = 2;
    
    if (slab_order != 0)
    {
        exp = base << (slab_order - 1);
    }
    else 
    {
        exp = 1;    
    }
    slab_size = exp * PAGESIZE;
    
    return slab_size;
}

static size_t get_slab_order(size_t object_size, size_t default_cnt)
{
    size_t base;
    size_t order;
    size_t total_size;

    base  = 2;
    order = 0;
    total_size = (object_size + sizeof(struct chunk)) * default_cnt + sizeof(struct slab);
    
    if (PAGESIZE >= total_size)
        return order;
    
    order++;
    while (PAGESIZE * base < total_size && order < 10)
    {
        order++;
        base <<= 1;
    }

    return order;
}

static size_t calculate_object_cnt(size_t slab_size, size_t object_size)
{
    size_t work_are_size = slab_size - sizeof(struct slab);
    size_t chunk_size    = object_size + sizeof(struct chunk);
    
    size_t new_cnt = work_are_size / chunk_size;
    return new_cnt;
}

void cache_setup(struct cache *cache, size_t object_size)
{
    if (!cache)
    {

    }

    slab_list_init(&(cache->free_slabs));
    slab_list_init(&(cache->partially_filled_slabs));
    slab_list_init(&(cache->filled_slabs));
 
    cache->object_size  = object_size;
    cache->slab_order   = get_slab_order(object_size, DEFAULT_SIZE);
    cache->slab_size    = calculate_slab_size(cache->slab_order);
    
    if (cache->slab_size < cache->object_size)
    {
        return;
    }
    
    cache->slab_objects = calculate_object_cnt(cache->slab_size, object_size);
}

void cache_release(struct cache *cache)
{
    /* Реализуйте эту функцию. */
}


void *cache_alloc(struct cache *cache)
{
    if (!cache)
    {
        return;
    }
    

    void *mem = NULL;
    if (cache->partially_filled_slabs.size == 0)
    {
        slab *slab = create_slab(cache->object_size, cache->slab_order, cache->slab_objects);
        add_to_list(&(cache->partially_filled_slabs), slab);
        mem = alloc_chunk(slab);
    }
    else if (cache->partially_filled_slabs.size > 0)
    {
        slab *slab = cache->partially_filled_slabs.head;
        mem = alloc_chunk(slab);
        if (is_full(cache, slab))
        {
            slab_reposition(&cache->partially_filled_slabs, &cache->filled_slabs, slab);
        }
    }
    return mem;
}

void cache_free(struct cache *cache, void *ptr)
{
    if (!cache)
    {
        return;
    }

    if (!ptr)
    {
        return;
    }    
    
    chunk      *chunk = (struct chunk*)((char*)ptr - sizeof(struct chunk));
    slab       *slab  = (struct slab*)chunk->slab_ptr;
    slab_list  *list  = (struct slab_list*)slab->list_ptr;
    
    /*printf("chunk %ld\n", chunk);
    printf("slab %ld\n", slab);
    printf("list %ld\n", list);
    */
    free_chunk(slab, chunk);
    
    if (slab->blck_cnt = 0)
    {
        //puts("cache_free 1");
        slab_reposition(list, &cache->free_slabs, slab);
    }
    else 
    {
        slab_reposition(list, &cache->partially_filled_slabs, slab);
    }
}

void cache_shrink(struct cache *cache)
{
    /* Реализуйте эту функцию. */
    if (!cache)
    {
        return;
    }
}
