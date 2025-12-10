#include <stdio.h>
#include "alloc.h"
#include "alloc_utils.h"

size_t* chunk_init_metainfo(size_t * chunk)
{
    return chunk + HEADER_SIZE;
}

void set_size(size_t * chunk, size_t size)
{
    size_t *size_ptr = chunk - SIZE_OFFSET;
    *size_ptr = size;
}

size_t get_size(size_t * chunk)
{
    size_t *size_ptr = chunk - SIZE_OFFSET;
    return *size_ptr;
}

void set_next_chunk_ptr(size_t * chunk, size_t *next_chunk)
{
    size_t **next = (size_t **)(chunk - NEXT_OFFSET);
    *next = next_chunk;
}

size_t * get_next_chunk_ptr(size_t * chunk)
{
    size_t **next = (size_t **)(chunk - NEXT_OFFSET); 
    return *next;
}

void set_prev_chunk_ptr(size_t * chunk, size_t * prev_chunk)
{
    size_t **prev = (size_t **)(chunk - PREV_OFFSET);
    *prev = prev_chunk;
}

size_t * get_prev_chunk_ptr(size_t * chunk)
{
    size_t **prev = (size_t **)(chunk - PREV_OFFSET);
    return *prev;
}

void set_left_border_marker(size_t * chunk, size_t flag)
{
    size_t *flag_ptr = chunk - L_BORDER_MARKER_OFFSET;
    *flag_ptr = flag;
}

size_t get_left_border_marker(size_t * chunk)
{
    return *(chunk - L_BORDER_MARKER_OFFSET);
}

void set_right_border_marker(size_t * chunk, size_t size, size_t flag)
{
  size_t *flag_ptr = chunk + size/sizeof(size_t *);
  *flag_ptr = flag;
}

size_t get_right_border_marker(size_t * chunk)
{
 
  return *(chunk + get_size(chunk)/sizeof(size_t *));
}

size_t * make_chunk(size_t *chunk, size_t size, size_t status)
{
    chunk = chunk_init_metainfo(chunk);
    set_size(chunk, size);
    set_next_chunk_ptr(chunk, NULL);
    set_prev_chunk_ptr(chunk, NULL);
    set_left_border_marker(chunk, status);
    set_right_border_marker(chunk, size, status);
    return chunk;
}

size_t * find_chunk(size_t *head, size_t size)
{
  size_t *chunk = head;
  do 
  {
    size_t chunk_size = get_size(chunk);
    size_t chunk_status = get_left_border_marker(chunk); 
    size_t chunk_status_r = get_right_border_marker(chunk);  
    if (chunk_size > size)
    {  
        return chunk;
    }
    
    chunk = get_next_chunk_ptr(chunk);
  } while(chunk);
    return NULL;
}

void add_next(size_t *l_chunk, size_t *r_chunk)
{
    size_t *cur_next_l_chunk = get_next_chunk_ptr(l_chunk);
    if (cur_next_l_chunk != NULL)
        set_prev_chunk_ptr(cur_next_l_chunk, r_chunk);
    
    set_next_chunk_ptr(r_chunk, cur_next_l_chunk);
    set_prev_chunk_ptr(r_chunk, l_chunk); 
    
    set_next_chunk_ptr(l_chunk, r_chunk);
    
}

void merge_chunks(size_t * l_chunk, size_t * r_chunk)
{
    set_right_border_marker(r_chunk, get_size(r_chunk), FREE);
    
    size_t * next_r_chunk = get_next_chunk_ptr(r_chunk);
    if (next_r_chunk)
        set_prev_chunk_ptr(next_r_chunk, l_chunk);
    
    set_left_border_marker(l_chunk, FREE);
    set_next_chunk_ptr(l_chunk, next_r_chunk);
    
    set_size(l_chunk, get_size(l_chunk) + get_size(r_chunk) + METAINFO_SIZE);
    
}

size_t get_offset_mask(size_t bytes)
{
    switch(bytes)
    {
        case 1:
            return ONE_BYTE_OFFSET;
        case 2:
            return TWO_BYTE_OFFSET;
        case 3:
            return THREE_BYTE_OFFSET;
        case 4:
            return FOUR_BYTE_OFFSET;
        case 5:
            return FIVE_BYTE_OFFSET;
        case 6:
            return SIX_BYTE_OFFSET;
        default:
            return SEVEN_BYTE_OFFSET;
    }
}

void setzero(void *ptr, size_t bytes_cnt)
{
    size_t *chunk_ptr = (size_t *)ptr;  
    
    size_t intpart_size = (bytes_cnt/sizeof(size_t))*sizeof(size_t);
    size_t otherpart_size = bytes_cnt - intpart_size; // always lesser than sizeof(type) in our case 8 bytes
    
    size_t *end_chunk_ptr = chunk_ptr + intpart_size/sizeof(size_t);
  
	size_t *cur_ptr = chunk_ptr;
	while(cur_ptr != end_chunk_ptr)
	{
	    *cur_ptr = 0;
	     cur_ptr = cur_ptr + 1;
	}
	
    if (otherpart_size != 0)
    {
        size_t offset_mask = get_offset_mask(otherpart_size);
        *cur_ptr &= offset_mask;
    }
   
}

void free_chunk(size_t * chunk)
{
    set_left_border_marker(chunk, FREE);
    set_right_border_marker(chunk, get_size(chunk), FREE);
}

void  print_list(size_t *head)
{
  size_t *chunk = head;
  do 
  {
    size_t chunk_size = get_size(chunk);
    size_t chunk_status = get_left_border_marker(chunk); 
    size_t chunk_status_r = get_right_border_marker(chunk); 
    printf("---------------------------------------------------------------\n");
    printf("chunk size: %d\n", chunk_size);
    printf("chunk status from left marker: %d\n", chunk_status); 
    printf("chunk status from right marker: %d\n", chunk_status_r);  
    
    chunk = get_next_chunk_ptr(chunk);
  } while(chunk);
    printf("---------------------------------------------------------------\n");
}
