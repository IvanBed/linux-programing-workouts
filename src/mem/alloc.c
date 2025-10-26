#include <sys/mman.h>
#include "alloc.h"
#include "alloc_utils.h"

static size_t* head;

void mysetup(void* buf, size_t size)
{
    head = make_block((size_t*)buf, size - METAINFO_SIZE, FREE);

}

void* myalloc(size_t size)
{
    size_t real_size = size + METAINFO_SIZE;
    size_t *block = find_block(real_size);
    if (!block)
        return NULL;
    size_t old_block_new_size = (get_size(block) - real_size);
    set_size(block, old_block_new_size);
    set_right_border_marker(block, old_block_new_size, FREE);
     
    size_t offset = old_block_new_size/sizeof(size_t*) + 1; 
    
    size_t *new_block = block + offset;  
    
    new_block = make_block(new_block, size, ALLOCATED);
      
    add_next(block, new_block);
    
    return new_block;
}

void myfree(void* p)
{
	if(!p)
        return;
    if(get_right_border_marker((size_t *)p) == FREE || get_left_border_marker((size_t *)p) == FREE )
        return;
	
    size_t *prev = get_prev_block_ptr((size_t *)p);
    size_t *next = get_next_block_ptr((size_t *)p);
    size_t *block = (size_t*) p;
    if (!prev && !next)
    {
        free_block(block);
    }
    else if (prev && !next)
    {
        if(get_right_border_marker(prev) == FREE)
        {
            merge_blocks(prev, block);
        }
        else 
        {
            free_block(block);
        }
    }
    else if (!prev && next)
    {
        if(get_left_border_marker(next) == FREE)
        {
            merge_blocks(block, next);
        }
        else 
        {
            free_block(block);
        }
    } 
    else 
    {
        if(get_left_border_marker(prev) == FREE && get_left_border_marker(next) == FREE)
        {
            merge_blocks(block, next);
            merge_blocks(prev, block);
            
        }
        else if (get_left_border_marker(prev) == FREE && get_left_border_marker(next) != FREE)
        {
            merge_blocks(prev, block);
            
        }
        else if (get_left_border_marker(prev) != FREE && get_left_border_marker(next) == FREE)
        {
            merge_blocks(block, next);
        }
        else
        {
            free_block(block);
        }       
    } 
}

//Main fo testing
int main() {
    
    size_t alloc_size = 4096;
    
    void *blc = mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (!blc)
    {
        printf("Could not get memory chunk from OS!!!\n");
        exit(1);
    }
    mysetup(blc, alloc_size);
    void *p1 = myalloc(64);
    void *p2 = myalloc(128);
    void *p3 = myalloc(256); 
    char *p4 = (char *) myalloc(1024);
    void *p5 = myalloc(2048);
    void *p6 = myalloc(64);
    void *p7 = myalloc(64);
    void *p8 = myalloc(32);
    
    myfree(p3);
    myfree(p4);
    myfree(p7);
    myfree(p8);
    myfree(p6);
    myfree(p5);
    myfree(p1);
    myfree(p2);
    
    p1 = myalloc(64);
    p2 = myalloc(128);
    p3 = myalloc(256); 
    p4 = (char *) myalloc(1024);
    p5 = myalloc(2048);
    p6 = myalloc(64);
    p7 = myalloc(64);
    p8 = myalloc(32);
    
    myfree(p3);
    myfree(p4);
    myfree(p7);
    myfree(p8);
    myfree(p6);
    myfree(p5);
    myfree(p1);
    myfree(p2);
        
    
    print_list();
    munmap(blc, alloc_size);
	exit(0);
}
