#include "include/conn_pool.h"

ConnectionsPool *create_conn_pool(size_t size)
{
    size_t mem_size;
    
    ConnectionsPool *store = (ConnectionsPool*) malloc(sizeof(ConnectionsPool));
    if (!store)
    {
        return 0;
    }
    memset(store, 0, sizeof(ConnectionsPool));
    
    mem_size = size * sizeof(ConnectionDesc);
    store->connections = (ConnectionDesc*)malloc(mem_size);
    if (!store->connections)
    {
        free(store);
        return 0;
    }  
    memset(store->connections, 0, mem_size);
    
    if (pthread_mutex_init (&(store->lock), NULL) != 0)
    {
        free(store);
        free(store->connections);
        return 0;
    }
    store->free_space_bitmap = INT64_MAX;
    store->size = size;
    return store;
}

void destruct_conn_pool(ConnectionsPool *store)
{
    if (!store)
    {

    }

    free(store->connections);
    pthread_mutex_destroy(&(store->lock));
    free(store);
}

static int64_t book_pos(int64_t *bitmap)
{
    if (!bitmap)
    {

    }
   
    int mask = 1;
    int64_t res = __builtin_ctz(*bitmap);
    
    mask <<= res;
    *bitmap &= ~mask;
    return res;
}

static void unbook_pos(int64_t *bitmap, int64_t offset)
{
    if (!bitmap)
    {

    }    
    int mask = 1;
    mask <<= offset;
    *bitmap |= mask;
}

int64_t store_connection(ConnectionsPool *store, pthread_t thread_id, int connection_sock)
{
    int64_t offset;
    
    if (!store)
    {

    }   

    offset = book_pos(&store->free_space_bitmap);
    if (offset == -1)
    {
        return NOT_OK;
    }
    
    ConnectionDesc conn;
    conn.thread_id = thread_id;
    conn.connection_sock = connection_sock;
    conn.offset = offset;
    
    memcpy(store->connections + offset, &conn, sizeof(ConnectionDesc));
  
    return offset;
}

int64_t release_connection(ConnectionsPool *store, int64_t offset)
{
    if (!store)
    {

    } 
    ConnectionDesc *conn = store->connections + offset;
    puts("!release_connection! unbook_pos");
    
    //shutdown(conn->connection_sock, SHUT_RDWR);
    close(conn->connection_sock);
    unbook_pos(&store->free_space_bitmap, offset);
    memset(conn, 0, sizeof(ConnectionDesc));
    return OK;
}

int bitmap_contain(int64_t bitmap, int64_t offset)
{
    int64_t mask;
    if (!bitmap)
    {

    }  
    mask = 1;
    mask <<= offset;
    mask &= bitmap;
    return mask == 0;
}

void print(ConnectionsPool *store)
{
    if (!store)
    {

    }  

    printf("store->size %ld\n", store->size);
    for (size_t i = 0; i < store->size; i++)
    {
        ConnectionDesc *cur_conn = store->connections + i;
        int64_t mask = 1;
        mask <<= i;
        //printf("i %ld\n", i);
        mask &= store->free_space_bitmap;
        if (cur_conn && bitmap_contain(store->free_space_bitmap, i))
        {
           printf("thread %d socket %d offset %ld\n", cur_conn->thread_id, cur_conn->connection_sock, cur_conn->offset);
        }    
    }
}

