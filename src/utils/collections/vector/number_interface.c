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


int main() 
{
    vector *test = NULL;
    vector_init(1, TYPE_64, &test);
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
