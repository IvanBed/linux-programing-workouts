
#ifndef LIST_H
#define LIST_H

#include "slab.h"

typedef struct slab_list
{
    size_t size;
    slab  *head;
} slab_list;

void slab_list_init(slab_list *);
void slab_list_free(slab_list *);
void add_to_list(slab_list *, slab *);
void delete_from_list(slab_list *, slab *);
void slab_reposition(slab_list *, slab_list *, slab *);

#endif
