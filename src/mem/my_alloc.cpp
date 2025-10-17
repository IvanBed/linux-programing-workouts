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
static void* head;
static void *max_ptr_val;
static std::size_t list_size;

static void set_size(void *, std::size_t);
static void set_next(void *, void *);
static void set_prev(void *, void *);

void mysetup(void* buf, std::size_t size)
{
    head = buf;
    list_size = size;
    set_size(buf, size);
}

// Функция аллокации
void* myalloc(std::size_t size)
{}

// Функция освобождения
void myfree(void* p)
{}


static void set_size(void * cur_node, std::size_t size)
{
    void* size_ptr = cur_node;
    *(std::size_t*)size_ptr = size;
}


static std::size_t get_size(void * cur_node)
{
    void* size_ptr = cur_node;
    return *(std::size_t*)size_ptr;
}


static void set_next(void * cur_node, void * next_node)
{
    void* next = cur_node + 1;
    next = next_node;
}


static void * get_next(void * cur_node)
{
    return cur_node + 1;
}


static void set_prev(void * cur_node, void * prev_node)
{
    void* prev = cur_node + 2;
    prev = prev_node;
}


static void * get_prev(void * cur_node)
{
    return cur_node + 2;
}


static void * get_data(void * cur_node)
{
    return cur_node + 3;
}


/*int main() {
    void* blc = malloc(1032);
    std::size_t size_info = 1024;

    void* size_ptr = blc;
    void* next = blc + 1;
    void* prev = blc + 2;
    blc = blc + 3;
    //*next = (blc + 32);


    *(std::size_t*)size_ptr = size_info;
    blc = (void*)((std::size_t*)blc + 1);


    std::cout << *((size_t*)size_ptr) << std::endl;
    std::cout << sizeof(void*) << std::endl;
}
*/
