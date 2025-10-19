#include <iostream>
#include <stdlib.h>

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
static void set_next(size_t *);
static void set_prev(size_t *, size_t *);
static void set_is_free_flag(size_t *, int);

void mysetup(void *buf, std::size_t size)
{
    head = (size_t*)buf;
    list_size = size;
    set_size((size_t*)buf, size);
}

// Функция аллокации
void * myalloc(std::size_t size)
{}

// Функция освобождения
void myfree(void* p)
{}


static size_t * block_init_metainfo(size_t * cur_block)
{
    return cur_block + 4;
}

static void set_size(size_t *cur_block, std::size_t size)
{
    size_t *size_ptr = cur_block - 4;
    *size_ptr = size;
}


static std::size_t get_size(size_t *cur_block)
{
    size_t *size_ptr = cur_block - 4;
    return *size_ptr;
}


static void set_next(size_t *cur_block)
{
    size_t *next = cur_block - 3;
    size_t offset_to_next = get_size(cur_block)/8  + 1;
    *next = offset_to_next;
}

static void set_next_null(size_t *cur_block)
{
    size_t *next = cur_block - 3;
    size_t offset_to_next = 0;
    *next = offset_to_next;
}

static size_t * get_next(size_t *cur_block)
{
    return (cur_block  - 3);
}


static size_t * get_next_block(size_t *cur_block)
{
    
    size_t offset = *(get_next(cur_block));
    if (offset == 0 )
    {
        std::cout << "offset is null !!! " << offset << "\n";
        return NULL;
    }
    
    cur_block = cur_block + offset;
    return cur_block;
}

static void set_next_block_ptr(size_t *cur_block, size_t *next_block)
{
    size_t **next = (size_t **)(cur_block - 3);
     
    *next = next_block;
}

static size_t *get_next_block_ptr(size_t *cur_block)
{
    size_t **next = (size_t **)(cur_block - 3);
     
    return *next;
}



static void set_prev_block(size_t *cur_block)
{
    size_t *prev = cur_block - 2;
    *prev = 0;
}


static void *get_prev(size_t *cur_block)
{
    return cur_block - 2;
}


static void set_is_free_flag(size_t *cur_block, int flag)
{
    size_t *flag_ptr = cur_block - 1;
    *flag_ptr = flag;
}


static size_t get_is_free_flag(size_t *cur_block)
{
    return *(cur_block - 1);
}


static void *get_data(size_t *cur_block)
{
    return (void *)(cur_block + 4);
}


static void find_block(std::size_t size)
{
  size_t *cur_block = head;
  do 
  {
    std::size_t cur_block_size = get_size(cur_block);
    std::size_t cur_block_status = get_is_free_flag(cur_block);
    std::cout << "Размер блока: " << cur_block_size << std::endl;
    std::cout << "Статус блока: " <<  cur_block_status << std::endl;
    if (cur_block_size > size && cur_block_status)
    {
      //выделяем этот блок
    }
    
    cur_block = get_next_block_ptr(cur_block);
  } while(cur_block);
    
}

int main() {
   
    size_t* blc = (size_t*)malloc(1024);
    head = blc;
    size_t *cur_ptr = blc;
    std::size_t size_info = 480;
   
    block_init_metainfo(cur_ptr);
    set_size(cur_ptr, size_info);
    set_prev_block(cur_ptr);
    set_is_free_flag(cur_ptr, 1);


    size_t *next_ptr = cur_ptr + (size_info/8 + 1);
    
    set_size(next_ptr, 224);
    set_prev_block(next_ptr);
    set_is_free_flag(cur_ptr, 1);
    
    
    set_next_block_ptr(cur_ptr, next_ptr);
    set_next_block_ptr(next_ptr, nullptr);
    
    find_block(32);
    
    /*cur_ptr = get_next_block(cur_ptr);
    
    block_init_metainfo(cur_ptr);
    set_size(cur_ptr, 224);
    set_next(cur_ptr);
    set_prev_block(cur_ptr);
    set_is_free_flag(cur_ptr, 0);
    
    
    cur_ptr = get_next_block(cur_ptr);
    
    block_init_metainfo(cur_ptr);
    set_size(cur_ptr, 56);
    set_next(cur_ptr);
    set_prev_block(cur_ptr);
    set_is_free_flag(cur_ptr, 1);
    

    cur_ptr = get_next_block(cur_ptr);
    
    block_init_metainfo(cur_ptr);
    set_size(cur_ptr, 123);
    set_next_null(cur_ptr);
    set_prev_block(cur_ptr);
    set_is_free_flag(cur_ptr, 1);

    
   
    
    char* test = (char* )get_data(cur_ptr);
    test = "abc";
    cout << test;
    */
}
