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

#define HELLO "hello"

size_t get_file_size(FILE *f)
{
    size_t size;
    
    fseek(f, 0, SEEK_END);  
    size = (size_t) ftell(f); 
    fseek(f, 0, SEEK_SET); 

    return size;
}

void print_file(char const * file_path)
{
    char   file_buf[PAGESIZE];
    FILE  *file;
    size_t file_size;
    size_t index;
    char   ch;
    
    file = fopen(file_path, "rb");
    
    if (!file)
    { 
        perror("Can not open file");
        return;
    }
    
    file_size = get_file_size(file);

    index = 0;
    while ((ch = fgetc(file)) != EOF)
    {
        if (index == PAGESIZE)
        {
            puts("");
            puts(file_buf);
            index = 0;
            memset(buf, 0, PAGESIZE);
        }
        printf("%c", ch); 
        file_buf[index++] = ch;
    }

    fclose(file);
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Specify the file path");
    }
    print_file(argv[1]);
}