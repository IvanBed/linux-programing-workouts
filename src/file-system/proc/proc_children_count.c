#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

#define FILE_SIZE 10 * 4096
#define PATH_MAX  4096
#define PPID_SIZE 1024
#define PPIDPATTERN "PPid:"

#define PROC_PATH "/proc"

bool read_file(FILE *fp, char *buf)
{
    size_t buf_index;
    for (buf_index = 0; buf_index < FILE_SIZE; buf_index++) 
    {
        buf[buf_index] = getc(fp);
    
        if (buf[buf_index] == EOF)
        { 
            buf[buf_index] = '\0';
            return true;
        }    
    }
    return false;
}

bool is_proc(char const *d_name)
{
    size_t i;
    size_t d_name_len = strlen(d_name);
    for (i = 0; i < d_name_len; i++)
    { 
        if (!('0' <= d_name[i] && d_name[i] <= '9'))
            return false;
    }
    return true;
}

bool get_proc_children(char const *pid, size_t *cnt)
{
    char path[PATH_MAX];
    char *file_content = (char *)malloc(FILE_SIZE);
    char *token, *save_point;
    sprintf(path, "/proc/%s/task/%s/children", pid, pid);  
    //puts(path);
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        perror("fopen() failed");
        return false;  
    }    
    if (read_file(fp, file_content))
    { 
        token = strtok_r(file_content, " ", &save_point);
        while (token != NULL)
        {
          //puts(token);
          if (is_proc(token))
              get_proc_children(token, cnt);
          
          token = strtok_r(NULL, " ", &save_point);
          
          (*cnt)++;  
        }  
        memset(file_content, 0, FILE_SIZE);        
    }    
    free(file_content);
    fclose(fp);
}

int main(int argc, char *argv[]) 
{
    if (argc != 2)
    {
        return 1;
    }
    size_t cnt = 1;
    get_proc_children(argv[1], &cnt);
    printf("%ld\n", cnt);
    return 0;
}
