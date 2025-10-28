// Вариант c++ для сдачи задания
#include <iostream>

#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#define FREE 1
#define ALLOCATED 0
#define METAINFO_SIZE 40

#define HEADER_SIZE 4 // count of cells that store the block's metainfo

#define SIZE_OFFSET 4
#define NEXT_OFFSET 3
#define PREV_OFFSET 2
#define L_BORDER_MARKER_OFFSET 1

#define ONE_BYTE_OFFSET   0b00001111111111111111111111111111;   
#define TWO_BYTE_OFFSET   0b00000000111111111111111111111111;   
#define THREE_BYTE_OFFSET 0b00000000000011111111111111111111;
#define FOUR_BYTE_OFFSET  0b00000000000000001111111111111111;
#define FIVE_BYTE_OFFSET  0b00000000000000000000111111111111;
#define SIX_BYTE_OFFSET   0b00000000000000000000000011111111;
#define SEVEN_BYTE_OFFSET 0b00000000000000000000000000001111;

/*
TO DO
    change size_t * to char *
	add WINAPI mem allocation and make cond compile
*/

using namespace std;

static size_t* head;

static size_t* block_init_metainfo(size_t * cur_block)
{
    return cur_block + HEADER_SIZE;
}

static void set_size(size_t * cur_block, std::size_t size)
{
    size_t *size_ptr = cur_block - SIZE_OFFSET;
    *size_ptr = size;
}

static size_t get_size(size_t * cur_block)
{
    size_t *size_ptr = cur_block - SIZE_OFFSET;
    return *size_ptr;
}

static void set_next_block_ptr(size_t * cur_block, size_t *next_block)
{
    size_t **next = (size_t **)(cur_block - NEXT_OFFSET);
    *next = next_block;
}

static size_t * get_next_block_ptr(size_t * cur_block)
{
    size_t **next = (size_t **)(cur_block - NEXT_OFFSET); 
    return *next;
}

static void set_prev_block_ptr(size_t * cur_block, size_t * prev_block)
{
    size_t **prev = (size_t **)(cur_block - PREV_OFFSET);
    *prev = prev_block;
}

static size_t * get_prev_block_ptr(size_t * cur_block)
{
    size_t **prev = (size_t **)(cur_block - PREV_OFFSET);
    return *prev;
}

static void set_left_border_marker(size_t * cur_block, size_t flag)
{
    size_t *flag_ptr = cur_block - L_BORDER_MARKER_OFFSET;
    *flag_ptr = flag;
}

static size_t get_left_border_marker(size_t * cur_block)
{
    return *(cur_block - L_BORDER_MARKER_OFFSET);
}

static void set_right_border_marker(size_t * cur_block, size_t size, size_t flag)
{
  size_t *flag_ptr = cur_block + size/sizeof(size_t);
  *flag_ptr = flag;
}

static size_t get_right_border_marker(size_t * cur_block)
{
 
  return *(cur_block + get_size(cur_block)/sizeof(size_t));
}

static size_t * make_block(size_t *block, size_t size, size_t status)
{
    block = block_init_metainfo(block);
    set_size(block, size);
    set_next_block_ptr(block, NULL);
    set_prev_block_ptr(block, NULL);
    set_left_border_marker(block, status);
    set_right_border_marker(block, size, status);
    return block;
}

static size_t * find_block(std::size_t size)
{
  size_t *cur_block = head;
  do 
  {
    std::size_t cur_block_size = get_size(cur_block);
    std::size_t cur_block_status = get_left_border_marker(cur_block); 
    std::size_t cur_block_status_r = get_right_border_marker(cur_block);  
    if (cur_block_size > size)
    {  
        return cur_block;
    }
    
    cur_block = get_next_block_ptr(cur_block);
  } while(cur_block);
    return NULL;
}

static void add_next(size_t *l_block, size_t *r_block)
{
    size_t *cur_next_l_block = get_next_block_ptr(l_block);
    if (cur_next_l_block != NULL)
        set_prev_block_ptr(cur_next_l_block, r_block);
    
    set_next_block_ptr(r_block, cur_next_l_block);
    set_prev_block_ptr(r_block, l_block); 
    
    set_next_block_ptr(l_block, r_block);
    
}

static void merge_blocks(size_t * l_block, size_t * r_block)
{
    set_right_border_marker(r_block, get_size(r_block), FREE);
    
    size_t * next_r_block = get_next_block_ptr(r_block);
    if (next_r_block)
        set_prev_block_ptr(next_r_block, l_block);
    
    set_left_border_marker(l_block, FREE);
    set_next_block_ptr(l_block, next_r_block);
    
    set_size(l_block, get_size(l_block) + get_size(r_block) + METAINFO_SIZE);
    
}

static void free_block(size_t * block)
{
    set_left_border_marker(block, FREE);
    set_right_border_marker(block, get_size(block), FREE);
}

void mysetup(void* buf, std::size_t size)
{
    head = make_block((size_t*)buf, size - METAINFO_SIZE, FREE);

}

void* myalloc(std::size_t size)
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
    size_t *cur_block = (size_t*) p;
    if (!prev && !next)
    {
        //std::cout << "!prev && !next" << std::endl;
        free_block(cur_block);
    }
    else if (prev && !next)
    {
        //std::cout << "prev && !next" << std::endl;
        if(get_right_border_marker(prev) == FREE)
        {
            //std::cout << "prev && !next merge" << std::endl;
            merge_blocks(prev, cur_block);
           
        }
        else 
        {
            free_block(cur_block);
        }
    }
    else if (!prev && next)
    {
        //std::cout << "!prev && next" << std::endl;
        if(get_left_border_marker(next) == FREE)
        {
            merge_blocks(cur_block, next);
        }
        else 
        {
            free_block(cur_block);
        }
    } 
    else 
    {
        //std::cout << "prev && next" << std::endl;
        if(get_left_border_marker(prev) == FREE && get_left_border_marker(next) == FREE)
        {
            //std::cout << "prev && next 1" << std::endl;
            merge_blocks(cur_block, next);
            merge_blocks(prev, cur_block);
            
        }
        else if (get_left_border_marker(prev) == FREE && get_left_border_marker(next) != FREE)
        {
           //std::cout << "prev && next 2" << std::endl;
            merge_blocks(prev, cur_block);
            
        }
        else if (get_left_border_marker(prev) != FREE && get_left_border_marker(next) == FREE)
        {
            //std::cout << "prev && next 3" << std::endl;
            merge_blocks(cur_block, next);
        }
        else
        {
            //std::cout << "prev && next 4" << std::endl;
            free_block(cur_block);
        }       
    } 
}

static void  print_list()
{
  size_t *cur_block = head;
  do 
  {
    std::size_t cur_block_size = get_size(cur_block);
    std::size_t cur_block_status = get_left_border_marker(cur_block); 
    std::size_t cur_block_status_r = get_right_border_marker(cur_block); 
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Block size: " << cur_block_size << std::endl;
    std::cout << "Block status from left marker: " <<  cur_block_status << std::endl;
    std::cout << "Block status from right marker: " <<  cur_block_status_r << std::endl;
    
    cur_block = get_next_block_ptr(cur_block);
  } while(cur_block);
    std::cout << "---------------------------------------------------------------\n";
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

void * mycalloc(size_t numitems, size_t size)
{
	size_t bytes_cnt = numitems*size;
	void * raw_chunk = myalloc(bytes_cnt);
	if (raw_chunk == NULL)
	    return NULL;
	setzero(raw_chunk, bytes_cnt); 
	
	return raw_chunk;
}

int main() {
    
    size_t alloc_size = 4096;
    
    void *blc = mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	//void *blc = malloc(alloc_size);
    if (!blc)
    {
        std::cout << "Could not get memory chunk from OS!!!\n";
        exit(1);
    }
    mysetup(blc, alloc_size);
    
    void *p1 = mycalloc(10, 7);
 
    /*void *p2 = myalloc(128);
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
    */
    //print_list();
    munmap(blc, alloc_size);
	exit(0);
}
