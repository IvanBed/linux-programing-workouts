#ifndef NET_H
#define NET_H

#define HELLO "hello\n"
#define ENOUGH 64
#define ERR_MSG1 "Can not open file\n"
#define OK_MSG "OK\n"
#define PAGESIZE 4096

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port);
void send_file(int connection_sock, char const * file_path);
void get_file(int connection_sock, char const * file_path, char const * dest_path);
int create_server(char *ip_address, int port);
#endif