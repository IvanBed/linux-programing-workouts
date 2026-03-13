#include "vector.h"

static uint8_t is_full(vector *inst)
{
    if (inst != NULL && inst->size >= inst->capacity)
        return true;
    else
        return false;
}

static uint8_t is_out_range(vector *inst,size_t index)
{
    if (inst != NULL && inst->size <= index)
        return true;
    else 
        return false;
}

static uint8_t resize(vector *inst)
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

static uint8_t insert(char *insert_pos, char *value, size_t type_size)
{
    if (insert_pos == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    for (size_t i = 0; i < type_size; i++)
    {
        insert_pos[i] = value[i];
    }
    return 0;
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
    if (!data)
    {
        free(new_vector);
        return ALLOC_ERR;
    }
    memset(data, 0, allocated_bytes);
    
    new_vector->data     = data;
    new_vector->size     = 0;
    new_vector->capacity = capacity;
    new_vector->rwlock   = NULL;
    
    *res_vector = new_vector;
    return NO_ERR;
}

uint8_t vector_destroy(vector *inst)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    free(inst->data);
    free(inst);
    return NO_ERR;
}

uint8_t vector_multithread_init(size_t capacity, size_t data_type_size, vector **res_vector)
{
    uint8_t init_res = vector_init(capacity, data_type_size, res_vector);
    if (init_res == NO_ERR)
    {
        if (pthread_rwlock_init((*res_vector)->rwlock, NULL) == -1)
        {
            return LOCK_INIT_ERR;
        }
    }    
    return init_res;
}

uint8_t vector_multithread_destroy(vector *inst)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    int destroy_res = pthread_rwlock_destroy(inst->rwlock);
    return vector_destroy(inst);
}

uint8_t add(vector *inst, char *el, size_t el_type_size)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    if (is_full(inst))
    {
        size_t tries = 0;
        while (resize(inst) != NO_ERR && tries++ < REALLOC_TRIES) {}
    }
  
    char *insert_pos = ((inst->data) + (inst->size * el_type_size));
    memcpy(insert_pos, el, el_type_size);
    inst->size = inst->size + 1;
    return NO_ERR;
}

uint8_t get(vector *inst, size_t index, size_t el_type_size, char *value)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    if (is_out_range(inst, index))
    {
        return OUT_OF_BOUND_ERR;
    }
    
    char *val_pos = ((inst->data) + (index * el_type_size));
    for (size_t i = 0; i < el_type_size; i++)
    {
        value[i] = val_pos[i];
    }
    
    return NO_ERR;
}

uint8_t add_multithread(vector *inst, char *el, size_t el_type_size)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    if (inst->rwlock == NULL)
    {
        return LOCK_INIT_ERR;
    }
    
    pthread_rwlock_wrlock(inst->rwlock);
    uint8_t res = add(inst, el, el_type_size);
    pthread_rwlock_unlock(inst->rwlock);

    return NO_ERR;
}

uint8_t get_multithread(vector *inst, size_t index, size_t el_type_size, char *value)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    if (inst->rwlock == NULL)
    {
        return LOCK_INIT_ERR;
    }
        
    pthread_rwlock_rdlock(inst->rwlock);
    uint8_t res = get(inst, index, el_type_size, value);
    pthread_rwlock_unlock(inst->rwlock);
    
    return res;
}