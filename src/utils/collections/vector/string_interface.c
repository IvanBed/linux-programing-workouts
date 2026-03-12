#include "number_interface.h"

uint8_t str_vector_init(size_t capacity, vector **res_vector)
{
    uint8_t init_res = vector_init(capacity, sizeof(char **), res_vector); 
    return init_res;
}

uint8_t str_vector_destroy(vector *inst)
{  
    return vector_destroy(inst);
}

uint8_t add_string(vector *inst, char *el)
{
    size_t  ptr_size = sizeof(char **);
    return add(inst, (char **)&el, ptr_size);
}

char * get_string(vector *inst, size_t index)
{
    size_t  ptr_size = sizeof(char **);
    char    string_ptr_buff[ptr_size]; 
    char    *el;
    
    uint8_t res = get_value(inst, index, ptr_size, string_ptr_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    
    el = *((char**)string_ptr_buff);    
    return el;
}