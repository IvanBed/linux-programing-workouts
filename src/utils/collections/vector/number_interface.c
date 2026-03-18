#include "number_interface.h"

uint8_t add_int(vector *inst, int el)
{
    int num = el;
    return add(inst, (char *)&num, TYPE_64);
}

uint8_t add_double(vector *inst, double el)
{
    double num = el;
    return add(inst, (char *)&num, TYPE_64);
}

int64_t get_int(vector *inst, size_t index)
{    
    char    num_buff[TYPE_64];
    int64_t el;
    
    uint8_t res = get(inst, index, TYPE_64, num_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    el = *((int*)num_buff);
    
    return el;
}

double get_double(vector *inst, size_t index)
{    
    char    num_buff[TYPE_64];
    double  el;
    
    uint8_t res = get(inst, index, TYPE_64, num_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    el = *((double*)num_buff);
    
    return el;
}
