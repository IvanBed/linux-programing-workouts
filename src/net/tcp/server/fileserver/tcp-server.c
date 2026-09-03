#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include <stdlib.h>
#include <errno.h>

#define LOOPBACK "127.0.0.1"
#define BUFSIZE 2 * 4096
#define PAGESIZE 4096
#define QEUEUSIZE 5

#define HELLO "hello\n"
#define ENOUGH 64

#define ERR_MSG1 "Can not open file\n"

void signal_handler(int signal_num)
{
    exit(EXIT_FAILURE);
}

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
}

size_t get_file_size(FILE *f)
{
    size_t size;

    fseek(f, 0, SEEK_END);
    size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

void send_file_old(int connection_sock, char const * file_path)
{
    char   file_buf[PAGESIZE];
    FILE  *file;
    size_t file_size;
    size_t index;
    char   ch;
    char   file_size_str[ENOUGH];

    file = fopen(file_path, "rb");

    if (!file)
    {
        send(connection_sock, ERR_MSG1, sizeof(ERR_MSG1), 0);
        perror("Can not open file");
        return;
    }

    file_size = get_file_size(file);
    sprintf(file_size_str, "%d\n", file_size);

    send(connection_sock, file_size_str, strlen(file_size_str), 0);
    index = 0;
    while ((ch = fgetc(file)) != EOF)
    {
        file_buf[index++] = ch;
        if (index == PAGESIZE)
        {
            send(connection_sock, file_buf, PAGESIZE, 0);
            index = 0;
            memset(file_buf, 0, PAGESIZE);
        }
    }
    send(connection_sock, file_buf, index, 0);
    //recv_cnt = read(connection_sock, buf, BUFSIZE);
    fclose(file);
}

void send_file(int connection_sock, char const * file_path)
{
    char   file_buf[PAGESIZE];
    FILE  *file;
    size_t file_size;
    size_t index;
    char   ch;
    char   file_size_str[ENOUGH];
    file = fopen(file_path, "rb");
    
    if (!file)
    {
        send(connection_sock, ERR_MSG1, sizeof(ERR_MSG1), 0);
        perror("Can not open file\n");
        return;
    }

    file_size = get_file_size(file);
    sprintf(file_size_str, "%d\n", file_size);
    send(connection_sock, file_size_str, strlen(file_size_str), 0);
    index = 0;

    while (fgets(file_buf, PAGESIZE, file) !=NULL)
    {
        if(send(connection_sock, file_buf, PAGESIZE, 0) == -1)
        {
            //log
        }
        memset(file_buf, 0, PAGESIZE);
    }

    //recv_cnt = read(connection_sock, buf, BUFSIZE);

    fclose(file);
}

void start_service(int connection_sock)
{
    ssize_t recv_cnt;
    char    file_path[BUFSIZE];

    memset(file_path, 0, BUFSIZE);

    recv_cnt = read(connection_sock, file_path, BUFSIZE);
    
    file_path[strlen(file_path) - 1] = '\0';
    send_file_old(connection_sock, file_path);
}

void main_loop(int server_sock)
{
    int     connection_sock;
    ssize_t recv_cnt;
    char    message[BUFSIZE];

    memset(message, 0, BUFSIZE);

    while (1)
    {
        listen(server_sock, QEUEUSIZE);
        connection_sock = accept(server_sock, NULL, NULL);
        puts("accepted!");

        send(connection_sock, HELLO, strlen(HELLO), 0);

        while ((recv_cnt = read(connection_sock, message, BUFSIZE)) > 0)
        {
             if (strcmp(message, "OFF\n") == 0)
                goto end_func;
            
            message[strlen(message) - 1] = '\0';
            send_file_old(connection_sock, message);
            memset(message, 0, BUFSIZE);
        }
    }

end_func:
    close(connection_sock);
}

int main(int argc, char **argv)
{
    int                server_sock;
    struct sockaddr_in local;
    int                port;
    int                bind_res;

    if (argc != 2)
    {
        perror("Specify the port");
        exit(EXIT_FAILURE);
    }

    signal(SIGURG, signal_handler);

    port = atoi(argv[1]);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    init_addr_ipinet(&local, LOOPBACK, port);

    bind_res = bind(server_sock, (struct sockaddr*) &local, sizeof(local));
    if (bind_res == -1)
    {
        perror("Can not bind the port");
        exit(EXIT_FAILURE);
    }
    puts("Start main loop!");
    main_loop(server_sock);

    close(server_sock);
    exit(EXIT_SUCCESS);
}