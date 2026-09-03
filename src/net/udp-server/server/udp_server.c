#include <stdio.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/stat.h>

#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <errno.h>

#define LOOPBACK "127.0.0.1"
#define BUFSIZE 2 * 4096

#define MAXPATH 4096

#define TRUE 1
#define FALSE 0

typedef int bool;

void signal_handler(int signal_num)
{
    exit(1);
}

void start_daemon()
{
    pid_t pid = fork();

    if (pid < 0)
    {
       exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0)
    {
       exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    pid = getpid();
    printf("[%d]\n", pid);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void init_addr_ipinet(struct sockaddr_in *addr, char *ip, int port)
{
    inet_aton(ip, &(addr->sin_addr));
    addr->sin_port = htons(port);
    addr->sin_family = AF_INET;
}

int get_random_num()
{
    srand(time(NULL));
    int rand_num = rand();
    return rand_num;
}

int open_log(char const *log_dir_path)
{
    char log_path[MAXPATH];
    int  log_fd;

    sprintf(log_path, "%s/log_file", log_dir_path);

    log_fd = open(log_path, O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);

    return log_fd;
}

bool validate_dir_path(char const *log_dir_path)
{
    DIR* dir = opendir(log_dir_path);
    if (dir)
    {
        closedir(dir);
        return TRUE;
    }
    return FALSE;
}

void main_loop(char const *log_dir_path, int sock_desc)
{
    char    buf[BUFSIZE];
    char   *message = "OK!";
    ssize_t recv_cnt;
    ssize_t log_write_cnt;
    int     log_fd;
    
    struct  sockaddr_in cliaddr;
    int     len;
    memset(buf, 0, BUFSIZE);

    log_fd = open_log(log_dir_path);

    if (log_fd == -1)
    {
        perror("Can not open file");
        return;
    }
    puts("Loop");
    while (1)
    {
        //recv_cnt = read(sock_desc, buf, BUFSIZE);
        recv_cnt = recvfrom(sock_desc, buf, BUFSIZE, 0, (struct sockaddr*)&cliaddr, &len);
        if (strncmp(buf, "OFF\n", 4) == 0)
            break;

        //printf("cnt:%d body:%s", recv_cnt, buf);
        log_write_cnt = write(log_fd, buf, recv_cnt);
        //write(sock_desc, message, sizeof(resp));
        sendto(sock_desc, message, strlen(message), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
        memset(buf, 0, BUFSIZE);
    }
    close(log_fd);
}

int main(int argc, char **argv)
{
    int                sock_desc;
    struct sockaddr_in local;
    int                port;
    int                bind_res;
    char              *log_dir_path;

    if (argc != 3)
    {
        perror("Specify the port and the log directory");
        exit(EXIT_FAILURE);
    }

    signal(SIGURG, signal_handler);

    port = atoi(argv[1]);
    log_dir_path = argv[2];

    if (!validate_dir_path(log_dir_path))
    {
        perror("Directory does not exist!");
        exit(EXIT_FAILURE);
    }
    //puts("Start daemon");
    start_daemon();
    sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    init_addr_ipinet(&local, LOOPBACK, port);

    bind_res = bind(sock_desc, (struct sockaddr*) &local, sizeof(local));
    main_loop(log_dir_path, sock_desc);
    close(sock_desc);

   exit(EXIT_SUCCESS);
}
