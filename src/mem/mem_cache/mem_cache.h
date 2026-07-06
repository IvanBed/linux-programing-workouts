
#ifndef MEMCACHE_H
#define MEMCACHE_H

#include "list.h"
#define DEFAULT_SIZE 10

struct cache 
{
    slab_list slabs_free;
    slab_list slabs_partial;
    slab_list slabs_full;


    size_t object_size; 
    int slab_order;
    size_t slab_objects;
    size_t slab_size;
};

#endif