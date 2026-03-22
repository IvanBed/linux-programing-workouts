#ifndef NUMBER_INTERFACE_H
    #define NUMBER_INTERFACE_H
    #include "vector.h"
    
    uint8_t int_vector_init(size_t, vector **);
    uint8_t int_vector_destroy(vector *);
    
    uint8_t int_vector_multithread_init(size_t , vector **);
    uint8_t int_vector_multithread_destroy(vector *inst);
    
    uint8_t double_vector_init(size_t, vector **);
    uint8_t double_vector_destroy(vector *);

    uint8_t add_int(vector *, int64_t, uint8_t);
    uint8_t add_double(vector *, double);
    int64_t get_int(vector *, size_t, uint8_t);
    double  get_double(vector *, size_t);
    uint8_t set_int(vector *, size_t, int64_t, uint8_t);

#endif