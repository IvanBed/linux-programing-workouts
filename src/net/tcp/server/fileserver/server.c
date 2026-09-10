/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>*/
#include "include/net.h"
#include "include/utils.h"
#include "include/os_layer.h"
#include "include/conn_pool.h"

#include <signal.h>
#include <errno.h>
#include <stdatomic.h>

#define LOOPBACK "127.0.0.1"
#define QEUEUSIZE 5
#define MAX_ATTEMPS 100
#define BUFSIZE 4096 * 2

#define THREAD_LIMIT 64

static atomic_int exit_signal = 0;

void signal_handler(int signal_num)
{
    puts("SIGINT signal recived. Terminated...");
    //exit_signal = 1;
    exit(EXIT_SUCCESS);
    puts("Flag exit_signal is TRUE.");
}

void start_service(int connection_sock)
{
    ssize_t        recv_cnt;
    char           request[BUFSIZE];
    char          *file_path;
    enum Operation op;

    char **        args;
    size_t         args_cnt;

    memset(request, 0, BUFSIZE);

    recv_cnt = read(connection_sock, request, BUFSIZE);
    args = parse_request(request, &args_cnt);
    op = get_op_type(args[0]);

    switch (op) 
    {
        case GET: 
            printf("request: %s | %s\n", args[0], args[1]);
            send_file(connection_sock, args[1]);
            break;
        case POST:
            get_file(connection_sock, args[1], args[2]);
            break;  
        default:
    }      
    free_args_list(args, args_cnt);
}

void* client_serving(void* data)
{
    WorkerArgs     *args;
    ConnectionDesc conn_desc;
    int64_t          release_res;

    if (!data)
    {

    }
    puts("client serving");
    args = (WorkerArgs*) data;
    conn_desc = args->connections_pool->connections[args->offset];
    puts("start service");
    start_service(conn_desc.connection_sock);
    pthread_mutex_lock(&(args->connections_pool->lock));
    puts("release connection");
    release_res = release_connection(args->connections_pool, conn_desc.offset);
    puts("pthread_mutex_unlock");
    pthread_mutex_unlock(&(args->connections_pool->lock));
    puts("pthread_mutex_unlock ok!");
    return 0;
}

void wait_pthreads(ConnectionsPool *pool)
{
    pthread_mutex_lock(&(pool->lock));
    for (size_t i = 0; i < pool->size; i++)
    {
        if (bitmap_contain(pool->free_space_bitmap, i))
            pthread_join(pool->connections[i].thread_id, 0);
    }
    pthread_mutex_unlock(&(pool->lock));
}

int64_t start_client_serving_routin(ConnectionsPool *pool, WorkerArgs *args, int connection_sock)
{
    pthread_t  thread_id;
    int64_t    res = OK;
    int64_t    offset;

    puts("start_client_serving_routin");
    if (!pool)
    {

    }

    pthread_mutex_lock(&(pool->lock));
    puts("store_connection");
    offset = store_connection(pool, thread_id, connection_sock);
    if (offset == NOT_OK)
    {
        res = offset;
        pthread_mutex_unlock(&(pool->lock));
        return NOT_OK;
    } 

    puts("pthread create");
    if (pthread_create(&thread_id, NULL, client_serving, (void*)(&args[offset])) != 0)
    {
        release_connection(pool, offset);
        pthread_mutex_unlock(&(pool->lock));
        return NOT_OK;
    }

    pthread_detach(thread_id);
    pthread_mutex_unlock(&(pool->lock));
    return res;
}

void init_worker_args(WorkerArgs *args, size_t thread_limit, ConnectionsPool *pool)
{
    memset(args, 0, thread_limit * sizeof(WorkerArgs));
    for (size_t i = 0; i < thread_limit; i++)
    {
        args[i].connections_pool = pool;
        args[i].offset = i;
    }
}

void main_loop(int server_sock, size_t max_connections)
{
    int               connection_sock;
    size_t            thread_indx = 0;
    size_t            attempts;
    ConnectionsPool   *pool;
    WorkerArgs        args[THREAD_LIMIT];

// Добавить правильное завершение!
    pool = create_conn_pool(max_connections);
    init_worker_args(args, THREAD_LIMIT, pool);
    listen(server_sock, QEUEUSIZE);
    puts("main_loop");
    while (!exit_signal)
    {
        attempts = 0;
        connection_sock = accept(server_sock, NULL, NULL);
        puts("accept");
        while (start_client_serving_routin(pool, args, connection_sock) == NOT_OK && attempts++ < MAX_ATTEMPS) {}
    }
    wait_pthreads(pool);
    destruct_conn_pool(pool);
}

int main(int argc, char **argv)
{
    int                server_sock;
    int                port;
    char              *ip_address;
    
    if (argc < 2)
    {
        perror("Specify the port");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    if (argc == 2)
        ip_address = LOOPBACK;
    else 
        ip_address = argv[2];

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
    
    server_sock = create_server(ip_address, port);
    if (server_sock == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    puts("Start main loop!");
    main_loop(server_sock, 16);
    
    //shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
    exit(EXIT_SUCCESS);
}