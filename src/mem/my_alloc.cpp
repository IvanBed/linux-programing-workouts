#include <iostream>

#include <unistd.h>
#include <sys/mman.h>

#define FREE 1
#define ALLOCATED 0
#define METAINFO_SIZE 40

/*
TO DO
    заменить size_t * на char *

*/

using namespace std;

static size_t* head;

static size_t* block_init_metainfo(size_t * cur_block)
{
    return cur_block + 4;
}

static void set_size(size_t * cur_block, std::size_t size)
{
    size_t *size_ptr = cur_block - 4;
    *size_ptr = size;
}

static std::size_t get_size(size_t * cur_block)
{
    size_t *size_ptr = cur_block - 4;
    return *size_ptr;
}

static void set_next_block_ptr(size_t * cur_block, size_t *next_block)
{
    size_t **next = (size_t **)(cur_block - 3);
    *next = next_block;
}

static size_t * get_next_block_ptr(size_t * cur_block)
{
    size_t **next = (size_t **)(cur_block - 3); 
    return *next;
}

static void set_prev_block_ptr(size_t * cur_block, size_t * prev_block)
{
    size_t **prev = (size_t **)(cur_block - 2);
    *prev = prev_block;
}

static size_t * get_prev_block_ptr(size_t * cur_block)
{
    size_t **prev = (size_t **)(cur_block - 2);
    return *prev;
}

static void set_left_border_marker(size_t * cur_block, size_t flag)
{
    size_t *flag_ptr = cur_block - 1;
    *flag_ptr = flag;
}


static size_t get_left_border_marker(size_t * cur_block)
{
    return *(cur_block - 1);
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
    set_next_block_ptr(block, nullptr);
    set_prev_block_ptr(block, nullptr);
    set_left_border_marker(block, status);
    set_right_border_marker(block, size, status);
    return block;
}

static size_t *  find_block(std::size_t size)
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

// Функция освобождения
void myfree(void* p)
{
    size_t *prev = get_prev_block_ptr((size_t *)p);
    if(!prev)
    {
        
    }
    size_t *next = get_next_block_ptr((size_t *)p);
    if(prev)
    {
        
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
  
    std::cout << "Размер блока: " << cur_block_size << std::endl;
    std::cout << "Статус блока: " <<  cur_block_status << std::endl;
    std::cout << "Статус блока: " <<  cur_block_status_r << std::endl;
    
    cur_block = get_next_block_ptr(cur_block);
  } while(cur_block);
}

int main() {
    
    size_t alloc_size = 2048;
    
    void *blc = mmap(0, alloc_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	//void *blc = malloc(alloc_size);
	
	mysetup(blc, alloc_size);
    void *p1 = myalloc(64);
    void *p2 = myalloc(128);
    void *p3 = myalloc(256); 
    void *p4 = myalloc(1024);
    void *p5 = myalloc(2048);
    print_list();

    munmap(blc, alloc_size);
	
}