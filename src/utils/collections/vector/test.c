#include "number_interface.h"

int main() 
{
    vector *test = NULL;
    vector_init(2, TYPE_64, &test);
    add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
    for (size_t i = 0; i < test->size; i++)
    {
        printf("%d\n", get_int(test, i));        
    }
  
}