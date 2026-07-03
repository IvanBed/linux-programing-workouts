#include "slab.h"
#define DEFAULT_SIZE 10

struct cache 
{
    /* список пустых SLAB-ов для поддержки cache_shrink */
    /* список частично занятых SLAB-ов */
    /* список заполненых SLAB-ов */
    slab_list slabs_free;
    slab_list slabs_partial;
    slab_list slabs_full;
    
    size_t object_size; /* размер аллоцируемого объекта */
    int slab_order; /* используемый размер SLAB-а */
    size_t slab_objects; /* количество объектов в одном SLAB-е */ 
    
    size_t slab_size;
};