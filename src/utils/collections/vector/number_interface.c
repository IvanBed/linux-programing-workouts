#include "number_interface.h"

// Init part

uint8_t int_vector_init(size_t capacity, vector **res_vector)
{
    uint8_t init_res = vector_init(capacity, sizeof(int64_t), res_vector); 
    return init_res;
}

uint8_t int_vector_destroy(vector *inst)
{  
    return vector_destroy(inst);
}

uint8_t int_vector_multithread_init(size_t capacity, vector **res_vector)
{
    uint8_t init_res = vector_multithread_init(capacity, sizeof(int64_t), res_vector); 
    return init_res;
}

uint8_t int_vector_multithread_destroy(vector *inst)
{  
    return vector_multithread_destroy(inst);
}

uint8_t double_vector_init(size_t capacity, vector **res_vector)
{
    uint8_t init_res = vector_init(capacity, sizeof(double), res_vector); 
    return init_res;
}

uint8_t double_vector_destroy(vector *inst)
{  
    return vector_destroy(inst);
}

// Interface part

uint8_t add_int(vector *inst, int64_t el, uint8_t multithread_flag)
{
    int num = el;
    if (!multithread_flag)
        return add(inst, (char *)&num, sizeof(int64_t));
    else
        return add_multithread(inst, (char *)&num, sizeof(int64_t));
}

uint8_t add_double(vector *inst, double el)
{
    double num = el;
    return add(inst, (char *)&num, sizeof(double));
}

int64_t get_int(vector *inst, size_t index, uint8_t multithread_flag)
{    
    size_t type_size = sizeof(int64_t);
    char    num_buff[type_size];
    int64_t el;
    
    uint8_t res;

    if (!multithread_flag)
        res = get(inst, index, type_size, num_buff);
    else
        res = get_multithread(inst, index, type_size, num_buff);

    if (res != NO_ERR)
    {
        printf("ERROR type: %d\n", res);
        return NO_EL;
    }
    el = *((int64_t*)num_buff);
    
    return el;
}

double get_double(vector *inst, size_t index)
{    
    size_t type_size = sizeof(double);
    char    num_buff[type_size];
    double  el;
    
    uint8_t res = get(inst, index, type_size, num_buff);
    if (res != NO_ERR)
    {
        printf("ERROR type: %d\n", res);
        return NO_EL;
    }
    el = *((double*)num_buff);
    
    return el;
}

uint8_t set_int(vector *inst, size_t index, int64_t el, uint8_t multithread_flag)
{
    int num = el;

    if (!multithread_flag)
        return set(inst, index, sizeof(int64_t), (char *)&num);
    else
        return set_multithread(inst, index, sizeof(int64_t), (char *)&num);
}

