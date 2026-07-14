#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <signal.h>

void signal_handler(int signal_num)
{
    exit(1);
}

void start_deamon()
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
    printf("%d\n", pid);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[])
{
    signal(SIGURG, signal_handler);
    
    start_deamon();
    while(1)
    {
        sleep(30);
    }
}
