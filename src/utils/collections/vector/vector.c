#include "vector.h"

uint8_t is_full(vector *inst)
{
    if (inst->size >= inst->capacity)
        return true;
    else
        return false;
}

uint8_t is_out_range(vector *inst,size_t index)
{
    if (inst->size <= index)
        return true;
    else 
        return false;
    
}

uint8_t vector_init(size_t capacity, size_t data_type_size, vector **res_vector)
{
    size_t allocated_bytes = capacity * data_type_size;
    
    vector *new_vector = (vector*) malloc(sizeof(vector));
    if (!new_vector)
    {
        return ALLOC_ERR;
    }
    
    char *data =  malloc(allocated_bytes);
    if (!new_vector)
    {
        free(new_vector);
        return ALLOC_ERR;
    }
    memset(data, 0, allocated_bytes);
    
    new_vector->data = data;
    new_vector->size = 0;
    new_vector->capacity = capacity;
    
    *res_vector = new_vector;
    return NO_ERR;
}

uint8_t resize(vector *inst)
{
    size_t new_capacity = inst->capacity * REALLOC_COEFFICIENT(inst->capacity);
    char *new_data      = realloc(inst->data, new_capacity);
    if (new_data == NULL)
    {
        return REALLOC_ERR;
    }
    inst->data     = new_data;
    inst->capacity = new_capacity;
    return NO_ERR;
}

uint8_t add(vector *inst, char *el, size_t data_type_size)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    if (is_full(inst))
    {
        //printf("resize!\n");
        size_t tries = 0;
        while (resize(inst) != NO_ERR && tries++ < REALLOC_TRIES) {}
        //printf("inst->size %d\n", inst->size);
        //printf("inst-> %d\n", inst->capacity);
    }
  
    
    char *place = *((inst->data) + (inst->size * data_type_size));
    //*((inst->data) + (inst->size * data_type_size))  = el;
    inst->size = inst->size + 1;
    return NO_ERR;
}

uint8_t add_int(vector *inst, int el)
{
    int num = el;
    return add(inst, (char *)&num, INT64_TYPE);
}

char *get(vector *inst, size_t index, size_t data_type_size)
{
    
    if (inst == NULL && is_out_range(inst, index))
    {
        return NULL;
    }
    return ((inst->data) + (index * data_type_size));
}

int main() 
{
    vector *test = NULL;
    vector_init(1, INT64_TYPE, &test);
    add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
    int *res1; 
    for (size_t i = 0; i < test->size; i++)
    {
        res1 = (int*) ((get(test, i, 8)));
        printf("%d\n", *res1);        
    }
  
}
