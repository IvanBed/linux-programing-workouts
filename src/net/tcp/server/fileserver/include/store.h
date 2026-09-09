#ifndef STORE_H

#define STORE_H

#include <pthread.h>

typdef struct ConnectionDesc 
{
    pthread_t       thread_id;
    int             connection_sock;
    size_t          store_pos;
} ConnectionDesc;

typdef struct ConnectionsStore
{
    ConnectionDesc *connections;
    size_t          size;
    pthread_mutex_t m;
    int64           free_pos;  
} ConnectionDesc;

#endif