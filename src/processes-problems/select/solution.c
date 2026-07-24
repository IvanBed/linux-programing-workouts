#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#define BUFF_SIZE 1024

#define F_PIPE "in1"
#define S_PIPE "in2"

#define BUFF_SIZE 1024

int parse_int(char const * int_str)
{
    int val;
    char* end_ptr;

    val = (int)atoi(int_str);
    return val;
}

int main()
{
    int    fds[2];
    char   buf[BUFF_SIZE];
    int    maxfd;
    fd_set watch_set;
    fd_set in_set;
    
    int sum = 0;
    
    if ((fds[0] = open(F_PIPE, O_RDONLY | O_NONBLOCK)) < 0)
    {
        
        return 1;
    }

    if ((fds[1] = open(S_PIPE, O_RDONLY | O_NONBLOCK)) < 0)
    {
        
        return 1;
    }
    
    FD_ZERO(&watch_set);
    FD_SET(fds[0], &watch_set);
    FD_SET(fds[1], &watch_set);    
    
    maxfd = fds[0] > fds[1]? fds[0] : fds[1];
    
    while(FD_ISSET(fds[0], &watch_set) || FD_ISSET(fds[1], &watch_set))
    {
        in_set = watch_set;
        if (select(maxfd + 1, &in_set, NULL, NULL, NULL) < 0)
        {
            
            return 1;
        }
        size_t i;
        for (i = 0; i < 2; i++)
        {
            if (FD_ISSET(fds[i], &in_set))
            {
                int bytes = read(fds[i], buf, BUFF_SIZE);
                if (bytes < 0)
                {
                    return 1;    
                }
                else if (bytes == 0)
                {
                    close(fds[i]);
                    FD_CLR(fds[i], &watch_set);
                }
                else 
                {
                    buf[bytes] = '\0';
                    sum += parse_int(buf);
                }
            }
        }
    }
    
    printf("%d\n", sum);
    return 0;
}