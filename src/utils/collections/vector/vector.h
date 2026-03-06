#ifndef VECTOR_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    
    #define REALLOC_COEFFICIENT(x) ((1024.0/(x + 128.0) + 1.0))  
    //ERROR_CODE
    #define NO_ERR 0
    #define ALLOC_ERR 1
    #define REALLOC_ERR 2
    #define NULL_PTR_ERR 3
    
	#define NO_EL -1
	
    #define REALLOC_TRIES 10
    
    #define 64_TYPE 64
    
    #define true 1 
    #define false 0
    
    typedef struct vector vector;
    
    struct vector
    { 
        char *data;
        size_t size;
        size_t capacity;
        
    };

#endif