#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX 8192


int main(int argc, char *argv[])
{
    FILE *pipe;
    char cmd[MAX];
    char result[MAX];
    char ch;
    size_t cnt = 0;

    if(argc != 3)
    {
        puts("err");
        return 1;
    }

    sprintf(cmd, "%s %s", argv[1], argv[2]);
    pipe = popen(cmd ,"r");

    while ((ch = fgetc(pipe)) != EOF)
    {
        if (ch == '0')
            cnt++;
    };
    printf("%ld\n", cnt);
    pclose(pipe);
}

