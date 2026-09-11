#include "include/conn_pool.h"

ConnectionsPool *create_conn_pool(size_t size)
{
    size_t mem_size;
    
    ConnectionsPool *pool = (ConnectionsPool*) malloc(sizeof(ConnectionsPool));
    if (!pool)
    {
        return 0;
    }
    memset(pool, 0, sizeof(ConnectionsPool));
    
    mem_size = size * sizeof(ConnectionDesc);
    pool->connections = (ConnectionDesc*)malloc(mem_size);
    if (!pool->connections)
    {
        free(pool);
        return 0;
    }  
    memset(pool->connections, 0, mem_size);
    
    if (pthread_mutex_init (&(pool->lock), NULL) != 0)
    {
        free(pool);
        free(pool->connections);
        return 0;
    }
    pool->free_space_bitmap = INT64_MAX;
    pool->size = size;
    return pool;
}

void destruct_conn_pool(ConnectionsPool *pool)
{
    if (!pool)
    {

    }

    free(pool->connections);
    pthread_mutex_destroy(&(pool->lock));
    free(pool);
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

int64_t store_connection(ConnectionsPool *pool, pthread_t thread_id, int connection_sock)
{
    int64_t offset;
    
    if (!pool)
    {

    }   
    if (pool->free_space_bitmap == 0)
    {
        puts("Pool is full");
        return NOT_OK;
    }

    offset = book_pos(&pool->free_space_bitmap);
    if (offset == -1 || offset >= pool->size)
    {
        return NOT_OK;
    }
    
    ConnectionDesc conn;
    conn.thread_id = thread_id;
    conn.connection_sock = connection_sock;
    conn.offset = offset;
    
    memcpy(pool->connections + offset, &conn, sizeof(ConnectionDesc));
  
    return offset;
}

int64_t release_connection(ConnectionsPool *pool, int64_t offset)
{
    if (!pool)
    {

    } 
    ConnectionDesc *conn = pool->connections + offset;
    puts("!release_connection! unbook_pos");
    
    //shutdown(conn->connection_sock, SHUT_RDWR);
    close(conn->connection_sock);
    unbook_pos(&pool->free_space_bitmap, offset);
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

void print(ConnectionsPool *pool)
{
    if (!pool)
    {

    }  

    printf("pool->size %ld\n", pool->size);
    for (size_t i = 0; i < pool->size; i++)
    {
        ConnectionDesc *cur_conn = pool->connections + i;
        int64_t mask = 1;
        mask <<= i;
        //printf("i %ld\n", i);
        mask &= pool->free_space_bitmap;
        if (cur_conn && bitmap_contain(pool->free_space_bitmap, i))
        {
           printf("thread %d socket %d offset %ld\n", cur_conn->thread_id, cur_conn->connection_sock, cur_conn->offset);
        }    
    }
}

