/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>*/
#include "include/net.h"
#include "include/utils.h"
#include "include/os_layer.h"
#include "include/conn_pool.h"

#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include <stdatomic.h>
#include <fcntl.h>

#define LOOPBACK "127.0.0.1"
#define QEUEUSIZE 5
#define MAX_ATTEMPS 100
#define BUFSIZE 4096 * 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define THREAD_LIMIT 64

// вынести в отдельный файл

typedef struct Pipe
{
    int    pfd[2];
    fd_set readfds;
    int    nfds;
} Pipe;

static atomic_int exit_signal = 0;

static Pipe *p; 

void signal_handler(int signal_num)
{
    puts("SIGINT signal recived. Terminated...");
    exit_signal = 1;
   
    int savedErrno;                 
    savedErrno = errno;
    if (write(p->pfd[1], "x", 1) == -1 && errno != EAGAIN)
        exit(EXIT_FAILURE);

    puts("Wrote to pipe");
    errno = savedErrno;
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
    //puts("client serving");
    args = (WorkerArgs*) data;
    conn_desc = args->connections_pool->connections[args->offset];
    //puts("start service");
    start_service(conn_desc.connection_sock);
    pthread_mutex_lock(&(args->connections_pool->lock));
    //puts("release connection");
    release_res = release_connection(args->connections_pool, conn_desc.offset);
    //puts("pthread_mutex_unlock");
    pthread_mutex_unlock(&(args->connections_pool->lock));
    //puts("pthread_mutex_unlock ok!");
    return 0;
}

void wait_pthreads(ConnectionsPool *pool)
{
    pthread_mutex_lock(&(pool->lock));
    while (!pool_is_free(pool)) 
    {
        pthread_cond_wait(&(pool->cond), &(pool->lock));
    }
    
    printf("bitmap %ld\n", pool->free_space_bitmap);
    printf("res %ld\n", pool->free_space_bitmap - INT64_MAX);
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
        return NOT_OK;
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

int64_t main_loop(int server_sock, size_t max_connections)
{
    int               connection_sock;
    size_t            thread_indx = 0;
    size_t            attempts;
    int               ready;
    int               nfds = 2;
    ConnectionsPool  *pool;
    WorkerArgs        args[THREAD_LIMIT];

    pool = create_conn_pool(max_connections);
    if (!pool)
    {
        perror("Can not create connections pool");
        return NOT_OK;
    }

    init_worker_args(args, THREAD_LIMIT, pool);
    listen(server_sock, QEUEUSIZE);
    
    FD_SET(server_sock, &(p->readfds)); 
    //Надо проверить эту тему
    p->nfds = MAX(p->nfds, server_sock + 1);
    puts("main_loop");

    while (!exit_signal)
    {
        attempts = 0;
        fd_set working_set;  
        memcpy(&working_set, &(p->readfds), sizeof(p->readfds));   
        puts("Select!");     
        while ((ready = select(p->nfds + 1, &working_set, NULL, NULL, NULL)) == -1 && errno == EINTR) 
        {
            continue;
        }

        if (FD_ISSET(p->pfd[0], &working_set)) 
        {  
            printf("A signal was caught\n");

            for (;;) 
            {                      /* Consume bytes from pipe */
                char ch;
                if (read(p->pfd[0], &ch, 1) == -1) 
                {
                    if (errno == EAGAIN)
                        break;              

                }
                printf("goto end_loop\n");
                goto end_loop;
            }
        }

        if (FD_ISSET(server_sock, &working_set)) 
        {
            connection_sock = accept(server_sock, NULL, NULL);
            //FD_SET(connection_sock, &working_set);
            if (connection_sock != -1) 
           {
                while (start_client_serving_routin(pool, args, connection_sock) == NOT_OK && attempts++ < MAX_ATTEMPS) {}
                puts("accept");
            }  
        }        
    }
end_loop:
    wait_pthreads(pool);
    destruct_conn_pool(pool);
    return OK;
}

int64_t init_pipe()
{
    p = malloc(sizeof(Pipe));
    FD_ZERO(&(p->readfds));
    if (pipe(p->pfd) == -1)
        return NOT_OK; 
    
    FD_SET(p->pfd[0], &(p->readfds));

    p->nfds = p->pfd[0] + 1;
    int flags = fcntl(p->pfd[0], F_GETFL);
    if (flags == -1)
    {
        perror("fcntl-F_GETFL");
        return NOT_OK; 
    }
        
    flags |= O_NONBLOCK;                
    if (fcntl(p->pfd[0], F_SETFL, flags) == -1)
    {
        perror("fcntl-F_GETFL");
        return NOT_OK; 
    }
    flags = fcntl(p->pfd[1], F_GETFL);
    if (flags == -1)
    {
        perror("fcntl-F_GETFL");
        return NOT_OK; 
    }
    flags |= O_NONBLOCK;                
    if (fcntl(p->pfd[1], F_SETFL, flags) == -1)
    {
        perror("fcntl-F_SETFL");
        return NOT_OK; 
    }
        
    return OK;
}

void free_pipe()
{
    free(p);
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


    init_pipe();
	
    //Переделать на новый обработчик
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
    free_pipe();
    close(server_sock);
    exit(EXIT_SUCCESS);
}