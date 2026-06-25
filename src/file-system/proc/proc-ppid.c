#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define PATH_MAX  4096
#define PPID_SIZE 1024
#define PPIDPATTERN "PPid:"

void get_ppid_from_line(char *str, size_t offset, char *res_buf)
{
    char *start = str;
    
    while(offset > 0)
    {
        offset--;
        start++;
    }    
    
    while((*start) == ' ' || (*start) == '\t' )
    {
        start++;
    }
        
    size_t len = strlen(start);
   
    size_t end = 0;
    while (end < len)
    {
        if (start[end] == ' ' || start[end] == '\t' || start[end] == '\0') 
        {
            break;
        }
        end++;
    }
    
    memcpy(res_buf, start, end);
    res_buf[end] = '\0';
}

bool find_substr(char const *str, char const *substr, size_t *offset)
{
    size_t l_ptr = 0;
    size_t r_ptr = 0;
    bool found   = false;
    
    size_t str_len    = strlen(str);
    size_t substr_len = strlen(substr);
    
//    printf("str %s\n", str);
//    printf("substr %s\n", substr);
    while (l_ptr < str_len)
    {
        if (str[l_ptr] == substr[r_ptr])
        {
            l_ptr++;
            r_ptr++;
           
        }
        else 
        {
            if (r_ptr > 0)
                r_ptr = 0;
            else 
                l_ptr++;
        }
        
        if (r_ptr >= substr_len)
        {
            found = true;
            *offset = l_ptr;
            break;
        }  
    }

    return found;    
    
}
// This func will crash on the file that contain last line without \n 
// must be amended
bool read_line(FILE *fp, char *buf)
{
    size_t buf_index;
    for (buf_index = 0; buf_index < PAGE_SIZE; buf_index++) 
    {
        buf[buf_index] = getc(fp);
    
        if (buf[buf_index] == '\n')
        { 
            buf[buf_index] = '\0';
            return true;
        }    
        
        if (buf[buf_index] == EOT)
        { 
            buf[buf_index] = '\0';
            return false;
        }    
    }
    return false;
}

bool read_status_file(FILE *fp, char *ppid)
{
    char line[PAGE_SIZE];

    while(read_line(fp, line))
    {
        size_t offset;
        if (find_substr(line, PPIDPATTERN, &offset))
        {
            get_ppid_from_line(line, offset, ppid);
            return true;
        }
        memset(line, 0, PAGE_SIZE);        
    }

    return false;
}

bool custom_get_ppid(int pid, char *ppid)
{
    char path[PATH_MAX]; 
	bool result = false;
    sprintf(path, "/proc/%d/status", pid);
    
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        perror("fopen() failed");
		return false;
    }
	
	result = read_status_file(fp, ppid);
	fclose(fp);
	
    return result;
    
}

int main() 
{
    char ppid[PPID_SIZE];
    int pid = getpid();
    if (custom_get_ppid(pid, ppid))
        printf("%s\n", ppid);

}

