#include "list.h"

void slab_list_init(slab_list *list)
{
    list->size = 0;
    list->head = NULL;
}

void slab_list_free(slab_list *list)
{
    if (!list)
    {
        printf("List is null\n"); 
        return;
    }
        
    if (list->size == 0)
    {
        printf("List size is zero\n"); 
        return;
    }
        
    slab *current_slab   = list->head;
    slab *temp           = NULL;
    while(current_slab)
    {
        temp = current_slab;
       
        current_slab = current_slab->prev_slab;
        delete_from_list(list, temp);
        free_slab((void*)temp);
    }
}

void add_to_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        printf("List is null\n"); 
        return;
    }

    if (!slab)
    {
        printf("Slab is null\n");
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
    slab->list_ptr = (void*) list;
}

void delete_from_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        printf("List is null\n"); 
        return;
    }

    if (!slab)
    {
        printf("Slab is null\n");
        return;
    }

    if (list->head == slab || list->size == 0)
    {
        list->head = list->head->prev_slab;
        if (list->head)
            list->head->next_slab = NULL;
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
