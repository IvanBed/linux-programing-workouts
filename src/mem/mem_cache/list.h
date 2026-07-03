#include "slab.h"

typedef struct slab_list 
{
    size_t size;
    slab  *head;
} slab_list;
