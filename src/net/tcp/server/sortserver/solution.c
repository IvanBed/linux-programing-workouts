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

#define ERR_MSG1 "Can not open file\n"

void signal_handler(int signal_num)
{
    exit(EXIT_FAILURE);
}

int compare(const void *a, const void *b) {
    char x = *(const char *)a;
    char y = *(const char *)b;

    if (x < y)
        return 1;
    if (x > y)
        return -1;
    
    return 0;
}

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
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
        while ((recv_cnt = read(connection_sock, message, BUFSIZE)) > 0)
        {
            if (strncmp(message, "OFF\n", 4) == 0)
                goto end_func;
               
            qsort(message, recv_cnt, sizeof(message[0]), compare);
            send(connection_sock, message, BUFSIZE, 0);
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
    
    main_loop(server_sock);
    close(server_sock);
    exit(EXIT_SUCCESS);
}
