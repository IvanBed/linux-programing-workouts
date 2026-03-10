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
    int64_t res;
    
    uint8_t res = get_value(inst, index, TYPE_64, num_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    res = *((int*)num_buff);
    
    return res;
}

double get_double(vector *inst, size_t index)
{    
    char    num_buff[TYPE_64];
    double  res;
    
    uint8_t res = get_value(inst, index, TYPE_64, num_buff);
    if (res != NO_ERR)
    {
        return NO_EL;
    }
    res = *((double*)num_buff);
    
    return res;
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
