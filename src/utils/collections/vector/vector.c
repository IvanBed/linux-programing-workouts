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

static uint8_t resize(vector *inst, size_t type_size)
{
    size_t new_capacity = (inst->capacity * REALLOC_COEFFICIENT(inst->capacity));
    char *new_data      = realloc(inst->data, new_capacity * type_size);
    
    if (new_data == NULL)
    {
        printf("can not realloc!");
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
    
    char *data = (char *) malloc(allocated_bytes);
    if (!data)
    {
        free(new_vector);
        return ALLOC_ERR;
    }
    memset(data, 0, allocated_bytes);
    
    new_vector->data      = data;
    new_vector->size      = 0;
    new_vector->capacity  = capacity;
    new_vector->lock_init = false;
    new_vector->realloc_process = false;

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
        
        if (pthread_rwlock_init(&((*res_vector)->rwlock), NULL) != 0)
        {
            return LOCK_INIT_ERR;
        }

        if (pthread_mutex_init(&((*res_vector)->mtx), NULL) != 0)
        {
            return MUTEX_INIT_ERR;
        }
        
        if (pthread_cond_init(&((*res_vector)->cond), NULL) != 0)
        {
            return COND_INIT_ERR;
        }

        (*res_vector)->lock_init = true;
        
    }    
    return init_res;
}

uint8_t vector_multithread_destroy(vector *inst)
{
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }
    
    int lock_destroy_res  = pthread_rwlock_destroy(&(inst->rwlock));
    int mtx_destroy_res   = pthread_mutex_destroy(&(inst->mtx));
    int cond_destroy_res  = pthread_cond_destroy(&(inst->cond));

    return vector_destroy(inst);
}

uint8_t clear(vector *inst, size_t el_type_size)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }
    memset(inst, 0, inst->size * el_type_size);
    inst->size = 0;
    
    return NO_ERR;
}

//Опасная функция, с точки зрения многопоточности
uint8_t clear_multithread(vector *inst, size_t el_type_size)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    pthread_rwlock_rdlock(&(inst->rwlock));
    while(inst->realloc_process) 
    {
        sched_yield();
    }
    pthread_rwlock_unlock(&(inst->rwlock));

    pthread_rwlock_wrlock(&(inst->rwlock));
    uint8_t res = clear(inst, el_type_size);
    pthread_rwlock_unlock(&(inst->rwlock));
}

uint8_t add(vector *inst, char *el, size_t el_type_size)
{
    if (!inst || !el)
    {
        return NULL_PTR_ERR;
    }
    
    if (is_full(inst))
    {
        size_t tries = 0;
        inst->realloc_process = true;
        printf("REALLOC!\n");
        while (resize(inst, el_type_size) != NO_ERR && tries++ < REALLOC_TRIES) {}
        inst->realloc_process = false;
    }
  
    char *insert_pos = ((inst->data) + (inst->size * el_type_size));
    memcpy(insert_pos, el, el_type_size);
    inst->size = inst->size + 1;
    return NO_ERR;
}

uint8_t get(vector *inst, size_t index, size_t el_type_size, char *value)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    if (inst->size == 0)
    {
        return ZERO_SIZE_VECTOR;
    }

    if (is_out_range(inst, index))
    {
        return OUT_OF_BOUND_ERR;
    }

    char *val_pos = ((inst->data) + (index * el_type_size));
    memcpy(value, val_pos, el_type_size);
    return NO_ERR;
}

uint8_t set(vector *inst, size_t index, size_t el_type_size, char *el)
{
    if (!inst || !el)
    {
        return NULL_PTR_ERR;
    }
    
    if (is_out_range(inst, index))
    {
        return OUT_OF_BOUND_ERR;
    }
    
    char *insert_pos = ((inst->data) + (index * el_type_size));
    memcpy(insert_pos, el, el_type_size);
    return NO_ERR;
}

uint8_t top(vector *inst, size_t el_type_size, char *value)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    if (inst->size == 0)
    {
        return ZERO_SIZE_VECTOR;
    }

    return get(inst, inst->size - 1, el_type_size, value);
}

uint8_t pop(vector *inst, size_t el_type_size, char *value)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    if (inst->size == 0)
    {
        return ZERO_SIZE_VECTOR;
    } 

    uint8_t res = top(inst, el_type_size, value);
    if (res == NO_ERR)
    {
        inst->size -= 1;
    }
    return res;
}

uint8_t add_multithread(vector *inst, char *el, size_t el_type_size)
{
    if (!inst || !el)
    {
        return NULL_PTR_ERR;
    }
    
    if (!inst->lock_init)
    {
        return LOCK_INIT_ERR;
    }
    pthread_rwlock_rdlock(&(inst->rwlock));
    while(inst->realloc_process) 
    {
        sched_yield();
    }
    pthread_rwlock_unlock(&(inst->rwlock));

    pthread_rwlock_wrlock(&(inst->rwlock));
    uint8_t res = add(inst, el, el_type_size);
    pthread_rwlock_unlock(&(inst->rwlock));

    return res;
}

uint8_t get_multithread(vector *inst, size_t index, size_t el_type_size, char *value)
{
    if (!inst || !value)
    {
        return NULL_PTR_ERR;
    }
    
    if (!inst->lock_init)
    {
        return LOCK_INIT_ERR;
    }
    
    pthread_rwlock_rdlock(&(inst->rwlock));
    while(inst->realloc_process) 
    {
        sched_yield();
    } 
    uint8_t res = get(inst, index, el_type_size, value);
    pthread_rwlock_unlock(&(inst->rwlock));
    
    return res;
}

uint8_t set_multithread(vector *inst, size_t index, size_t el_type_size, char *el)
{
    if (!inst || !el)
    {
        return NULL_PTR_ERR;
    }
    
    if (!inst->lock_init)
    {
        return LOCK_INIT_ERR;
    }
    
    pthread_rwlock_wrlock(&(inst->rwlock));
    while(inst->realloc_process) 
    {
        sched_yield();
    }
    uint8_t res = set(inst, index, el_type_size, el);
    pthread_rwlock_unlock(&(inst->rwlock));

    return res;
}

uint8_t top_multithread(vector *inst, size_t el_type_size, char *value)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    if (inst->size == 0)
    {
        return ZERO_SIZE_VECTOR;
    }

    return get_multithread(inst, inst->size - 1, el_type_size, value);
}

uint8_t pop_multithread(vector *inst, size_t el_type_size, char *value)
{
    if (!inst)
    {
        return NULL_PTR_ERR;
    }

    if (inst->size == 0)
    {
        return ZERO_SIZE_VECTOR;
    } 

    uint8_t res = top_multithread(inst, el_type_size, value);
    if (res == NO_ERR)
    {
        inst->size -= 1;
    }
    return res;
}