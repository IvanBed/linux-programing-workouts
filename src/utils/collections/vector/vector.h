#ifndef VECTOR_H
    
    #define VECTOR_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>

    #include <pthread.h>
    #include <sched.h>
    
    #define REALLOC_COEFFICIENT(x) ((1024.0/(x + 128.0) + 1.0))  
    
    //ERROR_CODE
    #define NO_ERR 0
    #define ALLOC_ERR 1
    #define REALLOC_ERR 2
    #define NULL_PTR_ERR 3
    #define OUT_OF_BOUND_ERR 4
    #define LOCK_INIT_ERR 5
    #define MUTEX_INIT_ERR 6
    #define COND_INIT_ERR 7


    #define NO_EL -1
    
    #define REALLOC_TRIES 10

    #define true 1 
    #define false 0
    
    typedef struct          vector vector;
    typedef unsigned char   raw_byte;
    typedef raw_byte        *raw_data;
    //typedef **raw_data      raw_data_ptr;
    
    struct vector
    { 
        char *data;
        size_t size;
        size_t capacity;
        pthread_rwlock_t rwlock;
        
        uint8_t realloc_process;
        pthread_mutex_t mtx;
        pthread_cond_t cond;

        uint8_t lock_init;
        
    };
    
    uint8_t vector_init(size_t, size_t, vector **);
    uint8_t vector_destroy(vector *);
    
    uint8_t vector_multithread_init(size_t, size_t, vector **);
    uint8_t vector_multithread_destroy(vector *);

    uint8_t add(vector *, char *, size_t);
    uint8_t get(vector *, size_t, size_t, char *);
    uint8_t set(vector *, size_t, size_t, char *);

    uint8_t add_multithread(vector *, char *, size_t );
    uint8_t get_multithread(vector *, size_t, size_t, char *);
    uint8_t set_multithread(vector *, size_t, size_t, char *);
    
#endif