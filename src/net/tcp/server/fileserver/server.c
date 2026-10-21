#include "include/server.h"
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

void serve_connection(ConnectionDesc *conn_desc, LogDesc *log_desc)
{
    ssize_t        recv_cnt;
    char           request[BUFSIZE];
    char          *file_path;
    enum Operation op;

    char **        args;
    size_t         args_cnt;

    memset(request, 0, BUFSIZE);

    recv_cnt = read(conn_desc->connection_sock, request, BUFSIZE);
    args = parse_request(request, &args_cnt);
    op = get_op_type(args[0]);
  
    //LOG_WRITE_THREAD(log_desc, tid, fprintf("request: %s | %s\n", args[0], args[1]);); 
    switch (op) 
    {
        case GET: 
            send_file(conn_desc->connection_sock, args[1]);
            break;
        case POST:
            get_file(conn_desc->connection_sock, args[1], args[2]);
            break;  
        default:
    }      
    puts("free_args_list");
    free_args_list(args, args_cnt);
}

void* client_serving(void* data)
{
    WorkerArgs     *args;

    ConnectionDesc conn_desc;
    LogDesc       *log_desc;
    int64_t        release_res;
    
    if (!data)
    {
        LOG_WRITE_ERROR_THREAD(log_desc, conn_desc.thread_id, "Data pointer in client_serving function is NULL, stop the routine");
        pthread_exit(NULL);
    }
    
    args      = (WorkerArgs*) data;
    conn_desc = args->connections_pool->connections[args->offset];
    log_desc  = args->log_desc;

    pid_t tid = *((pid_t*)(((char*)conn_desc.thread_id + 720)));

    LOG_WRITE_THREAD(log_desc, tid, "client serving"); 
    LOG_WRITE_THREAD(log_desc, tid, "start service"); 
    serve_connection(&conn_desc, log_desc);
    pthread_mutex_lock(&(args->connections_pool->lock));
    LOG_WRITE_THREAD(log_desc, tid, "release connection");
    release_res = release_connection(args->connections_pool, conn_desc.offset);
    LOG_WRITE_THREAD(log_desc, tid, "pthread_mutex_unlock");
    pthread_mutex_unlock(&(args->connections_pool->lock));
    
    pthread_exit(NULL);
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
    pthread_t  *thread_id_ptr;
    int64_t     offset;

    puts("start_client_serving_routin");
    if (!pool)
    {
        return NOT_OK;
    }
    
    puts("store_connection");
    offset = store_connection(pool, connection_sock);
    if (offset == NOT_OK)
    {
        return NOT_OK;
    } 

    thread_id_ptr = get_tid_pos(pool, offset);
    if (!thread_id_ptr)
    {
        return NOT_OK;
    }

    puts("pthread create");
    if (pthread_create(thread_id_ptr, NULL, client_serving, (void*)(&args[offset])) != 0)
    {
        release_connection(pool, offset);
        return NOT_OK;
    }

    pthread_detach(*thread_id_ptr);
    return OK;
}

void init_worker_args(WorkerArgs *args, size_t thread_limit, ConnectionsPool *pool, LogDesc *log_desc)
{
    memset(args, 0, thread_limit * sizeof(WorkerArgs));
    for (size_t i = 0; i < thread_limit; i++)
    {
        args[i].connections_pool = pool;
        args[i].offset = i;
        args[i].log_desc = log_desc;
    }
}

pthread_t start_log_routin(LogDesc *log_desc)
{
    pthread_t  thread_id;
    if (pthread_create(&thread_id, NULL, log_writer_routine, (void*)log_desc) != 0)
    {
        return NOT_OK;
    }

    return thread_id;
}

int64_t main_loop(int server_sock, size_t max_connections, LogDesc *log_desc)
{
    int               connection_sock;
    size_t            thread_indx = 0;
    size_t            attempts;
    int               ready;
    int               nfds = 2;
    ConnectionsPool  *pool;
    WorkerArgs        args[THREAD_LIMIT];
    int               start_client_serving_res;

    pool = create_conn_pool(max_connections);
    if (!pool)
    {
        perror("Can not create connections pool");
        //LOG_ERROR(log_desc, "Can not create connections pool");
        return NOT_OK;
    }

    init_worker_args(args, THREAD_LIMIT, pool, log_desc);
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
        //LOG_WRITE(log_desc, "Select!");
        while ((ready = select(p->nfds + 1, &working_set, NULL, NULL, NULL)) == -1 && errno == EINTR) 
        {
            continue;
        }

        if (FD_ISSET(p->pfd[0], &working_set)) 
        {  
            puts("A signal was caught");
            for (;;) 
            {
                char ch;
                if (read(p->pfd[0], &ch, 1) == -1) 
                {
                    if (errno == EAGAIN)
                        break;              

                }
                puts("goto end_loop");
                //LOG_WRITE(log_desc, "goto end_loop");
                goto end_loop;
            }
        }

        if (FD_ISSET(server_sock, &working_set)) 
        {
            puts("accept");
            connection_sock = accept(server_sock, NULL, NULL);
            //FD_SET(connection_sock, &working_set);
            if (connection_sock != -1) 
           {
                pthread_mutex_lock(&(pool->lock));
                while (pool_is_full(pool) /*&& attempts++ < MAX_ATTEMPS*/) 
                {
                    pthread_cond_wait(&(pool->is_free_cond), &(pool->lock));
                }

                start_client_serving_res = start_client_serving_routin(pool, args, connection_sock);
                pthread_mutex_unlock(&(pool->lock));
                
                //LOG_WRITE(log_desc, "accept");
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
    char               current_dir[PATH_SIZE];
    pthread_t          log_worker_tid;

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

    //Переделать на новый обработчик
    signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    int64_t res = init_pipe();
    if (res == NOT_OK)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
	 
    getcwd(current_dir, PATH_SIZE); 
    LogDesc *log_desc = create_log(current_dir, 1024);
    if (!log_desc)
    {
        perror("socket");
        free_pipe();
        exit(EXIT_FAILURE);
    }
    
    log_worker_tid = start_log_routin(log_desc);
    if (log_worker_tid == NOT_OK)
    {
        perror("pipe");
        free_pipe();
        destroy_log(log_desc);
        exit(EXIT_FAILURE);
    }    

    server_sock = create_server(ip_address, port);
    if (server_sock == -1)
    {
        perror("socket");
        free_pipe();
        destroy_log(log_desc);
        exit(EXIT_FAILURE);
    }
    puts("Start main loop!");
    
    //log_write(LogDesc *ld, char * const message, int error, log_level level)
    LOG_WRITE(log_desc, "Start main loop!");
    puts("go futher!!");
    main_loop(server_sock, 16, log_desc);
    
    LOG_WRITE(log_desc, "Stop main loop!"); 
    shutdown(server_sock, SHUT_RDWR);
    free_pipe();
    close(server_sock);
    log_desc->shutdown_flag = 1;
    pthread_join(log_worker_tid, NULL);
    destroy_log(log_desc);
    exit(EXIT_SUCCESS);
}
