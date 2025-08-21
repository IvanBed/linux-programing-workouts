#include <stdio.h>
#include <pthread.h>


struct Store {

    int *arr;
    size_t size;
    size_t head_pointer;
    size_t tail_pointer;
}


int store_init(struct Store **store_inst, size_t store_size) {

    *store_inst  = (struct Store*) malloc(sizeof(struct Store));
    if (*store_inst == NULL) {
         std::cout << "Could not allocate memory for structure of Store instance.\n";
         return 1;
    }
   *store_inst->size = store_size;
    int *arr_inst = (int*) malloc(sizeof(int) * store_size);
    if (arr_inst == NULL) {
         std::cout << "Could not allocate memory for store array instance.\n"
         return 1;
    }

   *store_inst->arr = arr_inst;
   *store_inst->head_pointer = store_size - 1;
   *store_inst->tail_pointer = store_size - 1;


    return 0;

}
