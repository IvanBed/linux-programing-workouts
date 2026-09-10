#ifndef CONNPOOL_H

#define CONNPOOL_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define OK 1
#define NOT_OK -1

typedef struct ConnectionDesc 
{
    pthread_t       thread_id;
    int             connection_sock;
    int64_t         offset;
} ConnectionDesc;

typedef struct ConnectionsPool
{
    ConnectionDesc *connections;
    size_t          size;
    pthread_mutex_t lock;
    int64_t         free_space_bitmap;  
} ConnectionsPool;

typedef struct WorkerArgs
{
    ConnectionsPool *connections_pool;
    int64_t          offset;
} WorkerArgs;

int64_t store_connection(ConnectionsPool *store, pthread_t thread_id, int connection_sock);
int64_t release_connection(ConnectionsPool *store, int64_t offset);
ConnectionsPool *create_conn_pool(size_t size);
void destruct_conn_pool(ConnectionsPool *store);
int bitmap_contain(int64_t bitmap, int64_t offset);

#endif