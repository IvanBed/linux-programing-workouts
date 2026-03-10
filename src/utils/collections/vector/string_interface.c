#include "number_interface.h"

uint8_t add_string(vector *inst, char const *el)
{
     
    return add(inst, (char *)&el, TYPE_64);
}