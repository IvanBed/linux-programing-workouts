#ifndef NUMBER_INTERFACE_H
    #define NUMBER_INTERFACE_H
    #include "vector.h"
    
    uint8_t int_vector_init(size_t, vector **);
    uint8_t int_vector_destroy(vector *);
    uint8_t double_vector_init(size_t, vector **);
    uint8_t double_vector_destroy(vector *);

    uint8_t add_int(vector *, int64_t);
    uint8_t add_double(vector *, double);
    int64_t get_int(vector *, size_t);
    double  get_double(vector *, size_t);
    
#endif