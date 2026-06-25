#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define PAGE_SIZE 4096
#define PATH_MAX  4096


#define PPIDPATTERN "PPid:"
#define NAMEPATTERN "Name:"

#define PROC_PATH "/proc"

void get_stat_val_from_line(char *str, size_t offset, char *res_buf)
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
        
        if (buf[buf_index] == EOF)
        { 
            buf[buf_index] = '\0';
            return false;
        }    
    }
    
    return false;
}

bool read_status_file(FILE *fp, char const *key, char *value)
{
    char line[PAGE_SIZE];

    while(read_line(fp, line))
    {
        size_t offset;
        if (find_substr(line, key, &offset))
        {
            get_stat_val_from_line(line, offset, value);
            return true;
        }
        memset(line, 0, PAGE_SIZE);        
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

bool get_proc_info_by_key(char const *key, size_t *count)
{
    char value[PAGE_SIZE];   
    char path[PATH_MAX];
    struct dirent *dir_entry = NULL;
    
    
    DIR *dir = opendir(PROC_PATH);
    if (dir == NULL)
    {
        printf("Could not open directory: %s\n", PROC_PATH);
        return false;
    }
    
    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (dir_entry->d_type == DT_DIR && is_proc(dir_entry->d_name))
        {
            memset(path, 0, PATH_MAX);
            memset(value, 0, PAGE_SIZE);
            sprintf(path, "%s/%s/status", PROC_PATH, dir_entry->d_name); 
            //printf("%s\n", path);
            
            FILE *fp = fopen(path, "r");
            if (!fp)
            {
                perror("fopen() failed");
                continue;
            }
              
            if (read_status_file(fp, key, value))
            {
                if (strcmp(value, "genenv") == 0)
                    (*count)++;
                
                //printf("%s\n", value);
            }
            
            fclose(fp);
        }
    }
    closedir(dir);
    return true;
}

int main() 
{
    size_t proc_cnt = 0;
    get_proc_info_by_key(NAMEPATTERN, &proc_cnt);
    
    printf("%ld\n", proc_cnt);
}
