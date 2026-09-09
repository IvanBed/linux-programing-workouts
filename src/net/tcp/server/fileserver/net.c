#include "include/filesystem.h"
#include "include/net.h"

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
}

int create_server(char *ip_address, int port)
{
    int    server_sock;
    struct sockaddr_in local;
    int    bind_res;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    init_addr_ipinet(&local, ip_address, port);

    bind_res = bind(server_sock, (struct sockaddr*) &local, sizeof(local));
    if (bind_res == -1)
    {
        perror("Can not bind the port");
        return -1;
    }
    return server_sock; 
}

//Добавить результат работы
void send_file(int connection_sock, char const * file_path)
{
    char      file_buf[PAGESIZE];
    char      resp[PAGESIZE];
    FileInfo *file_info;

    size_t    index;
    size_t    read_bytes;
    
    memset(resp, 0, PAGESIZE);
    memset(file_buf, 0, PAGESIZE);

    file_info = open_file(file_path);

    if (!file_info)
    {
        send(connection_sock, ERR_MSG1, sizeof(ERR_MSG1), 0);
        printf("%s]n", file_path);
        perror("Can not open file");
        return;
    } 
    else
    {
        puts("OK");
        send(connection_sock, OK_MSG, sizeof(OK_MSG), 0);
    }
    
    recv(connection_sock, resp, PAGESIZE, 0);
    if (strncmp(resp, "STOP\n", 4) == 0) 
    {
        puts("Stop send file to this client");
        return;
    }

    printf("File size %s\n", file_info->file_size_str); 
    send(connection_sock, file_info->file_size_str, strlen(file_info->file_size_str), 0);

    index = 0;
    puts("Send file"); 
    while (!feof(file_info->file))
    {
        if (ferror(file_info->file)) 
        {
            printf("File Error\n");
            break;
        }
        file_buf[index++] = (char)fgetc(file_info->file);
        if (index == PAGESIZE)
        {
            send(connection_sock, file_buf, PAGESIZE, 0);
            index = 0;
            memset(file_buf, 0, PAGESIZE);
            printf("Send %d bytes\n", index);
        }
    }
    
    if (index > 1)
        send(connection_sock, file_buf, index - 1, 0);
    
    printf("Send %d bytes\n", index - 1);
    close_file(file_info);
}

void get_file(int connection_sock, char const * file_path, char const * dest_path)
{


}

