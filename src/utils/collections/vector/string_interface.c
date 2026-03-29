#include "string_interface.h"

uint8_t str_vector_init(size_t capacity, vector **res_vector)
{
    uint8_t init_res = vector_init(capacity, sizeof(char **), res_vector); 
    return init_res;
}

uint8_t str_vector_destroy(vector *inst)
{  
    if (inst == NULL)
    {
        return NULL_PTR_ERR;
    }

    for (size_t i = 0; i < inst->size; i++)
    {
        char *str_ptr;
        get_string(inst, i, str_ptr);
        free(str_ptr);
    }
    return vector_destroy(inst);
}

static char const * copy_str(char *str)
{
    size_t str_len = strlen(str);

    char * new_str = (char *)malloc(str_len + 1);
    memcpy(new_str, str, str_len);
    new_str[str_len] = '\0';
    return new_str;
}

uint8_t add_string(vector *inst, char *el)
{
    size_t  ptr_size  = sizeof(char **);
    char * stored_str = copy_str(el);
    return add(inst, (char **)&stored_str, ptr_size);
}

uint8_t get_string(vector *inst, size_t index, char *str_res)
{
    size_t  ptr_size = sizeof(char **);
    printf("size of char** %d\n", ptr_size);
    char    string_ptr_buff[ptr_size]; 
    char    *el;
    
    uint8_t res = get(inst, index, ptr_size, string_ptr_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    el = *((char**)string_ptr_buff); 
    str_res = el;  
    return NO_ERR;
}
