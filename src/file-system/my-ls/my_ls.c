#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE 1024

#define true 1
#define false 0

#define NO_ERR 0
#define DIR_OPEN_ERR 1
#define GET_STAT_ERR 2
#define FILES_STAT_INIT_ERR 3
#define EMPTY_STR_ERR 4
#define READLINK_ERR 5

// Mod flag info

enum mod_flags
{
    VERBOSE = 1,
    ALL_FILES = 2
};

uint8_t empty(char const *str)
{
    if (str == NULL || strcmp(str, "") == 0)
        return true;
    else
        return false;
}

char const *make_absolute_path(char const *current_file_name, char const *prev_path)
{
    size_t prev_path_len        = strlen(prev_path);
    size_t current_dir_name_len = strlen(current_file_name);
    size_t len                  = current_dir_name_len + prev_path_len + 1;

    char *absolute_path  = (char *) malloc(len + 1);
    if (!absolute_path)
    {
        return NULL;
    }
    strcpy(absolute_path, prev_path);
    strcat(absolute_path, "/");
    strcat(absolute_path, current_file_name);

    return absolute_path;
}

#define NO_POS -1

int find_last_token_pos(char const *path)
{
    int path_len = (int)strlen(path);
    int start_pos = NO_POS;
    
    
    for (int i = path_len - 2; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            start_pos = i + 1;
            break;
        }   
    }
    return start_pos;
}

char const *get_file_name_slice(char const *absl_path)
{
    int start_pos = find_last_token_pos(absl_path);
    if (start_pos != NO_POS)
        return (absl_path + start_pos);
    else
        return absl_path;
}

uint8_t print_dir_entities_info(char const *dir_path, uint8_t mod)
{
    if (dir_path == NULL)
        return EMPTY_STR_ERR;

    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("Could not open directory: %s, please check syntex correctness\n", dir_path);
        return DIR_OPEN_ERR;
    }    
    
    struct       dirent *dir_entry   = NULL; 
    struct       stat file_stat;
    char         *next_file_path      = NULL;


    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (!MOD_ALL(mod) && strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) continue;
        
        next_file_path = make_absolute_path(dir_entry->d_name, dir_path);
        if (stat(next_file_path, &file_stat) != 0) return GET_STAT_ERR; 
        
        free(next_file_path);
    }
    
    closedir(dir);
    return NO_ERR;
}

uint8_t print_file_info(char const *file_path, uint8_t mod)
{
    if (empty(file_path))
        return EMPTY_STR_ERR;

    if (MOD_VERBOSE(mod))
        printf("%s\n", file_path);
    else 
        printf("%s\n", get_file_name_slice(file_path));
    
    return NO_ERR;
}

uint8_t print_s_link_info(char const *s_link_path, uint8_t mod)
{
    char    link_content_buffer[BUFFER_SIZE];
    ssize_t link_string_length;
    struct  stat file_stat;
    
    if ((link_string_length = readlink(s_link_path, link_content_buffer, sizeof (link_content_buffer))) == -1)
    {
        perror("readlink");
        return NO_ERR;
    }
    else
    {
        link_content_buffer[link_string_length] = '\0';
        
        if (MOD_VERBOSE(mod))
        {
            
            if (lstat(s_link_path, &file_stat) != 0) return GET_STAT_ERR;
            printf("");
        }
        else
        {
            printf("%s -> %s\n", s_link_path, link_content_buffer);
        }
    
    }

    return NO_ERR;
}

uint8_t parse_args(int argc, char **argv, char const **file_path, uint8_t *mod)
{

}

int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        return -1;
    }

    uint8_t mod           =  0;
    char const *file_path = NULL;
    
    
    return NO_ERR;
}