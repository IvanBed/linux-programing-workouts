#include <stdio.h>
#include "alloc.h"

size_t* block_init_metainfo(size_t * block)
{
    return block + HEADER_SIZE;
}

void set_size(size_t * block, size_t size)
{
    size_t *size_ptr = block - SIZE_OFFSET;
    *size_ptr = size;
}

size_t get_size(size_t * block)
{
    size_t *size_ptr = block - SIZE_OFFSET;
    return *size_ptr;
}

void set_next_block_ptr(size_t * block, size_t *next_block)
{
    size_t **next = (size_t **)(block - NEXT_OFFSET);
    *next = next_block;
}

size_t * get_next_block_ptr(size_t * block)
{
    size_t **next = (size_t **)(block - NEXT_OFFSET); 
    return *next;
}

void set_prev_block_ptr(size_t * block, size_t * prev_block)
{
    size_t **prev = (size_t **)(block - PREV_OFFSET);
    *prev = prev_block;
}

size_t * get_prev_block_ptr(size_t * block)
{
    size_t **prev = (size_t **)(block - PREV_OFFSET);
    return *prev;
}

void set_left_border_marker(size_t * block, size_t flag)
{
    size_t *flag_ptr = block - L_BORDER_MARKER_OFFSET;
    *flag_ptr = flag;
}

size_t get_left_border_marker(size_t * block)
{
    return *(block - L_BORDER_MARKER_OFFSET);
}

void set_right_border_marker(size_t * block, size_t size, size_t flag)
{
  size_t *flag_ptr = block + size/sizeof(size_t *);
  *flag_ptr = flag;
}

size_t get_right_border_marker(size_t * block)
{
 
  return *(block + get_size(block)/sizeof(size_t *));
}

size_t * make_block(size_t *block, size_t size, size_t status)
{
    block = block_init_metainfo(block);
    set_size(block, size);
    set_next_block_ptr(block, NULL);
    set_prev_block_ptr(block, NULL);
    set_left_border_marker(block, status);
    set_right_border_marker(block, size, status);
    return block;
}

size_t * find_block(size_t size)
{
  size_t *block = head;
  do 
  {
    size_t block_size = get_size(block);
    size_t block_status = get_left_border_marker(block); 
    size_t block_status_r = get_right_border_marker(block);  
    if (block_size > size)
    {  
        return block;
    }
    
    block = get_next_block_ptr(block);
  } while(block);
    return NULL;
}

void add_next(size_t *l_block, size_t *r_block)
{
    size_t *cur_next_l_block = get_next_block_ptr(l_block);
    if (cur_next_l_block != NULL)
        set_prev_block_ptr(cur_next_l_block, r_block);
    
    set_next_block_ptr(r_block, cur_next_l_block);
    set_prev_block_ptr(r_block, l_block); 
    
    set_next_block_ptr(l_block, r_block);
    
}

void merge_blocks(size_t * l_block, size_t * r_block)
{
    set_right_border_marker(r_block, get_size(r_block), FREE);
    
    size_t * next_r_block = get_next_block_ptr(r_block);
    if (next_r_block)
        set_prev_block_ptr(next_r_block, l_block);
    
    set_left_border_marker(l_block, FREE);
    set_next_block_ptr(l_block, next_r_block);
    
    set_size(l_block, get_size(l_block) + get_size(r_block) + METAINFO_SIZE);
    
}

void free_block(size_t * block)
{
    set_left_border_marker(block, FREE);
    set_right_border_marker(block, get_size(block), FREE);
}

void  print_list()
{
  size_t *block = head;
  do 
  {
    size_t block_size = get_size(block);
    size_t block_status = get_left_border_marker(block); 
    size_t block_status_r = get_right_border_marker(block); 
    printf("---------------------------------------------------------------\n");
    printf("Block size: %d\n", block_size);
    printf("Block status from left marker: %d\n", block_status); 
    printf("Block status from right marker: %d\n", block_status_r);  
    
    block = get_next_block_ptr(block);
  } while(block);
    printf("---------------------------------------------------------------\n");
}