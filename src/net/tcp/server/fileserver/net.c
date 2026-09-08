void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
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
        }

        file_buf[index++] = fgetc(file);
        if (index == PAGESIZE)
        {
            send(connection_sock, file_buf, PAGESIZE, 0);
            index = 0;
            memset(file_buf, 0, PAGESIZE);
            printf("Send %d bytes\n", PAGESIZE);
        }
    }
    send(connection_sock, file_buf, index, 0);
    printf("Send %d bytes\n", index);
    fclose(file);
}

void get_file(int connection_sock, char const * file_path, char const * dest_path)
{


}

int create_server(char const *ip, int port)
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
