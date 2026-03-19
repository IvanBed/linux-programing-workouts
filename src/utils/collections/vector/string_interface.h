#ifndef STRING_INTERFACE_H
    #define STRING_INTERFACE_H
    #include "vector.h"
    
    uint8_t str_vector_init(size_t, vector **);
    uint8_t str_vector_destroy(vector *);
    uint8_t add_string(vector *, char *);
    char * get_string(vector *, size_t);

#endif