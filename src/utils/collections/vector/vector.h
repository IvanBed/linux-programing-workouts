#ifndef VECTOR_H
    
    #define VECTOR_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>

    #define REALLOC_COEFFICIENT(x) ((1024.0/(x + 128.0) + 1.0))  
    //ERROR_CODE
    #define NO_ERR 0
    #define ALLOC_ERR 1
    #define REALLOC_ERR 2
    #define NULL_PTR_ERR 3
    
    #define NO_EL -1
    
    #define REALLOC_TRIES 10
    
    #define TYPE_64 64
    #define TYPE_32 32

    #define true 1 
    #define false 0
    
    typedef struct          vector vector;
    typedef unsigned char   raw_byte;
    typedef raw_byte        *raw_data;
    
    struct vector
    { 
        char *data;
        size_t size;
        size_t capacity;
        
    };
    
    uint8_t vector_init(size_t, size_t, vector **);
    uint8_t vector_destroy(vector *);
    
    uint8_t add(vector *, char *, size_t);
    uint8_t get_value(vector *, size_t, size_t, char *);
    
#endif