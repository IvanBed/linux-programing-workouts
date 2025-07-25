#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/errno.h>
#include <time.h>

void print_time() {
    time_t mytime = time(NULL);
    localtime(&mytime);
    char* time_frmd = ctime(&mytime);
    puts(time_frmd);
}

int main(int argc, char **argv) {

    puts("main process is started\n");
    printf("main process pid is %d\n", getpid());
    print_time();
    pid_t pids[4];
    int ch_pid;
    for (int i = 1; i < 5; i++) {

        char str[256];
        snprintf(str, sizeof(str), "sub_app_%s", argv[i]);
        char* sub_args[] = {str, NULL};
        if (!(ch_pid = fork())) {
            execl(str, str, NULL);
            fprintf(stderr, "Error executing %s: %s\n", str, strerror(errno));
           _exit(errno);
        } else {
            printf("pid of process %d\n", ch_pid);
            print_time();
            pids[i - 1] = ch_pid;
            sleep(1);
        }
    }

    puts("waiting for...\n");
    int stat;
    for (int i = 0; i < 4; ++i) {
        if (pids[i] > 0) {
            pid_t child_pid = waitpid(pids[i], &stat, 0);

            if (child_pid == -1) {
                perror("waitpid failed");
                continue;
            }

            if (WIFEXITED(stat))
                printf("Exit status: %d\n", WEXITSTATUS(stat));
            else if (WIFSIGNALED(stat))
                psignal(WTERMSIG(stat), "Exit signal");
            print_time();
        }
    }
    printf("main process finish ");
    print_time();
    return 0;
}