#include "list.h"

void slab_list_init(slab_list *list)
{
    list->size = 0;
    list->head = NULL;
}

void add_to_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        return;
    }

    if (!slab)
    {
        return;
    }
    
    if (list->size == 0 || !list->head)
    {
        list->head = slab;
        list->head->prev_slab = NULL;
        list->head->next_slab = NULL;
    }
    else 
    {
        slab->prev_slab = list->head;
        list->head->next_slab = slab;
        list->head = slab;
    }
    list->size++;
}

void delete_from_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        return;
    }

    if (!slab)
    {
        return;
    }
    
    if (list->head == slab)
    {
        list->head = list->head->prev_slab;
    }
    else 
    {
        struct slab *next = slab->next_slab;
        struct slab *prev = slab->prev_slab;

        if (prev)
            prev->next_slab = slab->next_slab;

        if (next)
            next->prev_slab = slab->prev_slab;
        
    }

    slab->next_slab = NULL;
    slab->prev_slab = NULL;
    list->size--;
}

void slab_reposition(slab_list *src, slab_list *dest, slab *slab)
{
    delete_from_list(src, slab);
    add_to_list(dest, slab);
}