#include <iostream>
#include <stdlib.h>
#define BORDER_MARKES_CNT 2


using namespace std;

// Эта функция будет вызвана перед тем как вызывать myalloc и myfree
    // используйте ее чтобы инициализировать ваш аллокатор перед началом
    // работы.
    //
    // buf - указатель на участок логической памяти, который ваш аллокатор
    //       должен распределять, все возвращаемые указатели должны быть
    //       либо равны NULL, либо быть из этого участка памяти
    // size - размер участка памяти, на который указывает buf
    
static size_t* head;
static size_t *max_ptr_val;
static std::size_t list_size;

static void set_size(size_t *, std::size_t);
static void set_is_free_flag(size_t *, int);

void mysetup(void* buf, std::size_t size)
{
    /*head = (size_t*)buf;
    list_size = size;
    set_size((size_t*)buf, size);*/
}

// Функция аллокации
void* myalloc(std::size_t size)
{}

// Функция освобождения
void myfree(void* p)
{}


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
    
}


static size_t * get_prev_block_ptr(size_t * cur_block)
{
    return nullptr;
}


static void set_is_free_flag(size_t * cur_block, int flag)
{
    size_t *flag_ptr = cur_block - 1;
    *flag_ptr = flag;
}


static size_t get_is_free_flag(size_t * cur_block)
{
    return *(cur_block - 1);
}


static void * get_data(size_t * cur_block)
{
    return (void *)(cur_block + 4);
}


static size_t make_offset_to_next(size_t * block)
{
    return get_size(next_ptr)/sizeof(size_t) + BORDER_MARKES_CNT;
}


static void find_block(std::size_t size)
{
  size_t *cur_block = head;
  do 
  {
    std::size_t cur_block_size = get_size(cur_block);
    std::size_t cur_block_status = get_is_free_flag(cur_block);  
    if (cur_block_size > size && cur_block_status)
    {  
        
        std::cout << "Размер блока: " << cur_block_size << std::endl;
        std::cout << "Статус блока: " <<  cur_block_status << std::endl;
    }
    
    cur_block = get_next_block_ptr(cur_block);
  } while(cur_block);
    
}


static size_t * make_block(size_t *block, size_t size, size_t status)
{
    block = block_init_metainfo(block);
    set_size(block, size);
    set_next_block_ptr(block, nullptr);
    set_prev_block_ptr(block, nullptr);
    set_is_free_flag(block, status);
    return block;
}

//Пока тестирую связный список, что реализован внутри непрерывного участка логической памяти
int main() {
   
    size_t *blc = (size_t*)malloc(1024);
    
    size_t *cur_ptr = blc;
    size_t *temp = nullptr;
    std::size_t size_info = 480;
    

    cur_ptr = block_init_metainfo(cur_ptr);
    head = cur_ptr;
    set_size(cur_ptr, size_info);
    set_is_free_flag(cur_ptr, 1);


    size_t *next_ptr = cur_ptr + make_offset_to_next(cur_ptr);
    next_ptr = block_init_metainfo(next_ptr);
    set_size(next_ptr, 224);
    
    set_is_free_flag(next_ptr, 1);
    
    set_next_block_ptr(cur_ptr, next_ptr);
    set_next_block_ptr(next_ptr, nullptr);
    
    temp = next_ptr;
    next_ptr = next_ptr + make_offset_to_next(next_ptr);
    next_ptr = make_block(next_ptr, 54, 1);
    set_next_block_ptr(temp, next_ptr);
    set_next_block_ptr(next_ptr, nullptr);
   
    temp = next_ptr;
    next_ptr = next_ptr + make_offset_to_next(next_ptr);
    next_ptr = make_block(next_ptr, 124, 1);
    set_next_block_ptr(temp, next_ptr);
    set_next_block_ptr(next_ptr, nullptr);
    
    find_block(158);
    
    int* test = (int*)get_data(next_ptr);
    *test = 123;
    cout << *test;
   
}
