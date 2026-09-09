/*#include <stdio.h>
#include <string.h>
#include <stdlib.h>*/
#include "include/net.h"
#include "include/utils.h"
#include "include/os_layer.h"

#include <signal.h>
#include <errno.h>

#define LOOPBACK "127.0.0.1"
#define QEUEUSIZE 5

static atomic_int exit_signal = 0;

void signal_handler(int signal_num)
{
    puts("SIGINT signal recived. Terminated...");
    exit_signal = 1;
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

void main_loop(int server_sock)
{
    int     connection_sock;

    while (!exit_signal)
    {
        listen(server_sock, QEUEUSIZE);
        connection_sock = accept(server_sock, NULL, NULL);
        puts("accepted!");
        start_service(connection_sock);
        close(connection_sock);
    }
}

void* start_service_multithread(void* data)
{
    int *server_sock_ptr;
    int  server_sock;
    
    server_sock_ptr = (int*) data;
    server_sock = *server_sock_ptr;
    start_service(server_sock);
    close(connection_sock);
}

void main_loop_multithread(int server_sock, size_t max_connections)
{
    int       connection_sock;
    size_t    thread_indx = 0;

    while (!exit_signal)
    {
        while (has_threads(t_store))
        {
            listen(server_sock, QEUEUSIZE);
            connection_sock = accept(server_sock, NULL, NULL);

            pthread_create(&get_thread(t_store), NULL, start_service_multithread, &connection_sock);
            thread_indx++;

        }

    }
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

	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);
    
    server_sock = create_server(ip_address, port);
    if (server_sock == -1)
    {
        exit(EXIT_FAILURE);
    }
    puts("Start main loop!");
    main_loop(server_sock);

    close(server_sock);
    exit(EXIT_SUCCESS);
}