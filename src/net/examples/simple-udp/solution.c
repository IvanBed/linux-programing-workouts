#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/uio.h>
#include <stdlib.h>

#define LOOPBACK "127.0.0.1"
#define BUFSIZE 2 * 4096

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
}

void main_loop(int sock_desc)
{
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);
    while (1)
    {
        read(sock_desc, buf, BUFSIZE);
        if (strncmp(buf, "OFF\n", 4) == 0)
            break;

        printf("%s\n", buf);
        memset(buf, 0, BUFSIZE);
    }
}

int main(int argc, char **argv)
{
    int                sock_desc;
    struct sockaddr_in local;
    int                port;
    int                bind_res;

    if (argc != 2)
    {
        puts("args!");
        return 1;
    }
	  
	port = atoi(argv[1]);
    
    sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    init_addr_ipinet(&local, LOOPBACK, port);

    bind_res = bind(sock_desc, (struct sockaddr*) &local, sizeof(local));
    main_loop(sock_desc);
    close(sock_desc);
	
    return 0;
}