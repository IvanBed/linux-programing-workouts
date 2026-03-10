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
#define PATH_MAX    4096 


#define true 1
#define false 0

#define NO_ERR 0
#define DIR_OPEN_ERR 1
#define GET_STAT_ERR 2
#define FILES_STAT_INIT_ERR 3
#define EMPTY_STR_ERR 4
#define READLINK_ERR 5
#define NOT_FLAG_ARG 6
//Args errors
#define NO_ARG_ERR 7

#define NO_POS -1

#define MOD_VERBOSE(mod) mod & 1 
#define MOD_ALL(mod) mod & 2 

// TO DO
// Фикс бага с флагами

enum mod_flag
{
    VERBOSE = 1,
    ALL
};

uint8_t empty(char const *str)
{
    if (str == NULL || strcmp(str, "") == 0)
        return true;
    else
        return false;
}

uint8_t make_mod_flag(char const *mod_str)
{
    uint8_t mod_flag = 0;
    size_t mod_str_len = strlen(mod_str);
    
    for (size_t i = 0; i < mod_str_len; i++)
    {
        switch(mod_str[i])
        {
            case 'a':
               mod_flag |= VERBOSE;
               break;
            case 'l':
               mod_flag |= ALL;
               break;     
            default:
        }
    }
    
    return mod_flag;
}

char const *make_absolute_path(char const *current_file_name, char const *prev_path)
{
    size_t prev_path_len        = strlen(prev_path);
    size_t current_dir_name_len = strlen(current_file_name);
    size_t len                  = current_dir_name_len + prev_path_len;

    char *absolute_path  = (char *) malloc(len + 1);
    if (!absolute_path)
    {
        return NULL;
    }
    strcpy(absolute_path, prev_path);
    //strcat(absolute_path, "/");
    strcat(absolute_path, current_file_name);

    return absolute_path;
}

char * const get_current_dir()
{
	char * current_dir_buff = (char*) malloc(PATH_MAX);
	if (!current_dir_buff)
	{
		return NULL;
	}
	
	
}

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

uint8_t print_file_info(char const *file_path, uint8_t mod)
{

	if (MOD_VERBOSE(mod))
	{
		printf("%s\n", file_path);
	}   
    else
	{
		printf("%s\n", get_file_name_slice(file_path));
	}
     
    return NO_ERR;
}

uint8_t print_s_link_info(char const *s_link_path, uint8_t mod)
{
    char    link_content_buffer[BUFFER_SIZE];
    size_t  link_string_length;

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
            printf("VERBOSE %s -> %s\n", s_link_path, link_content_buffer);
        }
        else
        {
            printf("%s -> %s\n", get_file_name_slice(s_link_path), link_content_buffer);
        }
    }

    return NO_ERR;
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
    
    struct dirent *dir_entry   = NULL; 
    char   *next_file_path     = NULL;
    
    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (MOD_ALL(mod) != 0 && strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) continue;
        
        next_file_path = make_absolute_path(dir_entry->d_name, dir_path);
        print_file_info(next_file_path, mod);
        free(next_file_path);
    }
    
    closedir(dir);
    return NO_ERR;
}

uint8_t parse_args(int argc, char ** argv, char **path, uint8_t *mod)
{
	if (argc < 2)
	{
		*mod = 0;
		*path = get_current_dir();
		if (*path == NULL)
		    return EMPTY_STR_ERR;
		else 
		    return NO_ERR;
	}
	else if (argc == 2)
	{
		*path = argv[1];
		*mod = 0;
		return NO_ERR; 
	}
	else
	{
	    if (argv[1][0] != '-')
	        return NOT_FLAG_ARG;
		else 
			*mod = make_mod_flag(argv[1]);
		
		*path = argv[2];	
	}
	
}

void get_info(char const *path, uint8_t mod)
{
	struct stat file_stat;
	if (lstat(path, &file_stat) == -1)
	{
		printf("ERROR!\n");
		return;
	}
	
	if (S_ISDIR(file_stat.st_mode))
	{
		print_dir_entities_info(path, mod);
	}
	else if (S_ISLNK(file_stat.st_mode))
	{
		print_s_link_info(path, mod);
	}
	else
	{
		print_file_info(path, mod);
	}
	
}

int main(int argc, char ** argv)
{
    
	uint8_t mod  = 0;
	char   *path = NULL;
	uint8_t err  = parse_args(argc, argv, &path, &mod);
	switch(err)
	{
		case EMPTY_STR_ERR:
			printf("my_ls: Unable to get current directory\n");
			return EMPTY_STR_ERR;
		case NOT_FLAG_ARG:
			printf("my_ls: The specified flag or flags do not exist\n");
			return NOT_FLAG_ARG;	
		default:					
	}	

    //struct stat file_stat;
	puts(path);
	get_info(path, mod);
	
    return NO_ERR;
}