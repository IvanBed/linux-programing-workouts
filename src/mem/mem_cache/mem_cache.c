#include "mem_cache.h"

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
    slab_size = exp * PAGE_SIZE;

    return slab_size;
}

static size_t get_slab_order(size_t object_size, size_t default_cnt)
{
    size_t base;
    size_t order;
    size_t total_size;

    base  = 2;
    order = 0;
    total_size = (object_size + sizeof(struct block)) * default_cnt + sizeof(struct slab);

    if (PAGE_SIZE >= total_size)
        return order;

    order++;
    while (PAGE_SIZE * base < total_size && order < 10)
    {
        order++;
        base <<= 1;
    }

    return order;
}

static size_t calculate_object_cnt(size_t slab_size, size_t object_size)
{
    size_t new_cnt = 0;
    
    size_t work_are_size = slab_size - sizeof(struct slab);
    size_t block_size    = object_size + sizeof(struct block);
    
    if (block_size != 0)
        new_cnt = work_are_size / block_size;

    return new_cnt;
}

void cache_setup(struct cache *cache, size_t object_size)
{
    slab_list_init(&(cache->slabs_free));
    slab_list_init(&(cache->slabs_partial));
    slab_list_init(&(cache->slabs_full));

    cache->object_size  = object_size;
    cache->slab_order   = get_slab_order(object_size, DEFAULT_SIZE);
    cache->slab_size    = calculate_slab_size(cache->slab_order);
    cache->slab_objects = calculate_object_cnt(cache->slab_size, object_size);
}

void cache_release(struct cache *cache)
{
    if (cache == NULL)
    {
        printf("Memory cache pointer is null\n");
        return;
    }
    slab_list_free(&(cache->slabs_free));
    slab_list_free(&(cache->slabs_partial));
    slab_list_free(&(cache->slabs_full));    
}

void *cache_alloc(struct cache *cache)
{
    assert(cache != NULL);
    
    void *mem  = NULL;
    slab *slab = NULL;
    if (cache->slabs_free.size > 0)
    {
        slab = cache->slabs_free.head;
        mem = alloc_block(slab);

        if (slab_is_full(slab))
        {
            slab_reposition(&cache->slabs_free, &cache->slabs_full, slab);
        }
        else
        {
            slab_reposition(&cache->slabs_free, &cache->slabs_partial, slab);
        }
    }
    else
    {
        if (cache->slabs_partial.size == 0)
        {
            slab = create_slab(cache->object_size, cache->slab_order, cache->slab_objects);
            add_to_list(&(cache->slabs_partial), slab);

        }
        else if (cache->slabs_partial.size > 0)
        {
            slab = cache->slabs_partial.head;
        }
        mem = alloc_block(slab);
        if (slab_is_full(slab))
        {
            slab_reposition(&cache->slabs_partial, &cache->slabs_full, slab);
        }
    }
    return mem;
}

void cache_free(struct cache *cache, void *ptr)
{
    if (cache == NULL)
    {
        printf("Memory cache pointer is null\n");
        return;
    }

    if (cache == NULL)
    {
        printf("Data pointer is null\n");
        return;
    }
    
    block      *block = (struct block*)((char*)ptr - sizeof(struct block));
    slab       *slab  = (struct slab*)block->slab_ptr;
    slab_list  *list  = (struct slab_list*)slab->list_ptr;

    free_block(slab, block);

    if (slab->size == 0)
    {
        slab_reposition(list, &cache->slabs_free, slab);
    }
    else
    {
        slab_reposition(list, &cache->slabs_partial, slab);
    }
}

void cache_shrink(struct cache *cache)
{
    if (cache == NULL)
    {
        printf("Memory cache pointer is null\n");
        return;
    }
    slab_list_free(&cache->slabs_free);
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