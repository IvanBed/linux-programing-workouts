#include "include/os_layer.h"

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