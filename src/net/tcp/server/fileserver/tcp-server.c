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
#define OK_MSG "OK\n"

enum Operation 
{
    GET,
    POST,
    UNDEFINED
};


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

void send_file(int connection_sock, char const * file_path)
{
    char   file_buf[PAGESIZE];
    char   resp[PAGESIZE];

    FILE  *file;
    char  *file_name;
    size_t file_size;
    size_t index;
    size_t read_bytes;
    char   file_size_str[ENOUGH];

    memset(resp, 0, PAGESIZE);
    memset(file_buf, 0, PAGESIZE);

    file = fopen(file_path, "rb");

    if (!file)
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
    
    read(connection_sock, resp, PAGESIZE);
    puts("resp");
    puts(resp);
    if (strncmp(resp, "STOP\n", 4) == 0) 
    {
        puts("Stop send file to this client");
        return;
    }

    file_size = get_file_size(file);
    sprintf(file_size_str, "%d\n", file_size);
    printf("File size %s\n", file_size_str); 
    send(connection_sock, file_size_str, strlen(file_size_str), 0);

    index = 0;
    puts("Send file"); 
    while (!feof(file))
    {
        if (ferror(file)) 
        {
            printf("File Error\n");
            break;
        }
        file_buf[index++] = (char)fgetc(file);
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
    fclose(file);
}

void get_file(int connection_sock, char const * file_path, char const * dest_path)
{


}

enum Operation get_op_type(char *const request) 
{
    if (strncmp(request, "GET", 3) == 0) 
        return GET;
    if (strncmp(request, "GET", 3) == 0) 
        return POST;
    return UNDEFINED;
}

size_t get_tokens_cnt(char const *str, size_t len) 
{
    size_t cnt = 1;
    for (size_t i = 0 ;i < len; i++)
        if (str[i] == ':')
            cnt++;
    return cnt;
}

void add_elemnt(char **tokens, char *str, size_t tokens_indx, size_t start_pos, size_t end_pos)
{
    size_t token_size = end_pos - start_pos;
    tokens[tokens_indx] = malloc(token_size);
    memcpy(tokens[tokens_indx], str + start_pos, token_size);
    tokens[tokens_indx][token_size] = 0;
    if (tokens[tokens_indx][token_size - 1]  == '\n') 
    {
        tokens[tokens_indx][token_size - 1] = 0;
    }
}

char **parse_request(char *str, size_t *out_token_cnt)
{
    size_t token_size;
    int    str_len = strlen(str); 
    size_t tokens_cnt = get_tokens_cnt(str, (size_t)str_len);
    
    char **tokens = malloc(sizeof(char*) * tokens_cnt);
    for (size_t i = 0; i < tokens_cnt;  i++)
    {
        tokens[i] = 0;
    }
    size_t start_pos = 0;
    size_t tokens_indx = 0;
    size_t end_pos = 0;
    for (; end_pos < str_len; end_pos++) 
    {
        if (str[end_pos] == ':' && tokens_indx < tokens_cnt)
        {
            add_elemnt(tokens, str, tokens_indx, start_pos, end_pos);
            start_pos = end_pos + 1;
            tokens_indx++;
        }
    }
    add_elemnt(tokens, str, tokens_indx, start_pos, end_pos);
    
    *out_token_cnt = tokens_cnt;
    return tokens;
}

void free_args_list(char **args_list, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(args_list[i]);
    }
    free(args_list);
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
        start_service(connection_sock);
        close(connection_sock);
    }

end_func:
    close(connection_sock);
}

int main(int argc, char **argv)
{
    int                server_sock;
    int                port;
    char              *ip_address;
    
    if (argc != 2)
    {
        perror("Specify the port");
        exit(EXIT_FAILURE);
    }

    signal(SIGURG, signal_handler);
    
    ip_address = LOOPBACK;
    port = atoi(argv[1]);

    /*server_sock = socket(AF_INET, SOCK_STREAM, 0);
    init_addr_ipinet(&local, ip_address, port);

    bind_res = bind(server_sock, (struct sockaddr*) &local, sizeof(local));
    if (bind_res == -1)
    {
        perror("Can not bind the port");
        exit(EXIT_FAILURE);
    }*/

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