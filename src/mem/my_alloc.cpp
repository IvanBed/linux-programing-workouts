
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
void mysetup(void* buf, std::size_t size)
{}

// Функция аллокации
void* myalloc(std::size_t size)
{}

// Функция освобождения
void myfree(void* p)
{}
//  выделяем 32 байта
int main() {
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
