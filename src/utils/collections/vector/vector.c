#include "vector.h"

static uint8_t is_full(vector *inst)
{
    if (inst->size >= inst->capacity)
        return true;
    else
        return false;
}

static uint8_t is_out_range(vector *inst,size_t index)
{
    if (inst->size <= index)
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
    
    new_vector->data = data;
    new_vector->size = 0;
    new_vector->capacity = capacity;
    
    *res_vector = new_vector;
    return NO_ERR;
}

uint8_t vector_destroy(vector *inst)
{
    free(inst->data);
    free(inst);
    return NO_ERR;
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
    insert(insert_pos, el, el_type_size); 
    inst->size = inst->size + 1;
    return NO_ERR;
}

uint8_t get_value(vector *inst, size_t index, size_t el_type_size, char *value)
{
    if (inst == NULL && is_out_range(inst, index))
    {
        return NULL_PTR_ERR;
    }
    
    char *val_pos = ((inst->data) + (index * el_type_size));
    for (size_t i = 0; i < el_type_size; i++)
    {
        value[i] = val_pos[i];
    }
    
    return NO_ERR;
}
