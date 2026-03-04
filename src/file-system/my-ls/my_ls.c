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

#define NO_ERR 0
#define DIR_OPD_ERR 1
#define GET_STAT_ERR 2
#define FILES_STAT_INIT_ERR 3
#define EMPTY_DIR_PATH_ERR 4
#define READLINK_ERR 5

char const *make_absolute_path(char const *current_dir_name, char const *prev_path)
{
    size_t prev_path_len        = strlen(prev_path);
    size_t current_dir_name_len = strlen(current_dir_name);
    size_t len                  = current_dir_name_len + prev_path_len + 1;

    char *absolute_path  = (char *) malloc(len + 1);
    if (!absolute_path)
    {
        return NULL;
    }
    strcpy(absolute_path, prev_path);
    strcat(absolute_path, "/");
    strcat(absolute_path, current_dir_name);

    return absolute_path;
}

uint8_t print_dir_entities_info(char const *dir_path, uint8_t mod)
{
    if (dir_path == NULL)
        return EMPTY_DIR_PATH_ERR;

    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("Could not open directory: %s, please check syntex correctness\n", dir_path);
        return DIR_OPD_ERR;
    }	
	
	struct       dirent *dir_entry   = NULL; 
	struct       stat file_stat;
	
	while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) continue;
        
		next_file_path = make_absolute_path(dir_entry->d_name, dir_path);
		if (stat(next_file_path, &file_stat) != 0) return GET_STAT_ERR; 
		
		
    }
	
	closedir(dir);
	return NO_ERR;
}

uint8_t print_file_info(char const *file_path, uint8_t mod)
{
	
	
	return NO_ERR;
}

uint8_t print_s_link_info(char const *s_link_path, uint8_t mod)
{
	char    buffer[BUFFER_SIZE];
	ssize_t link_string_length;
	struct  stat file_stat;
	
	if ((link_string_length = readlink(your_filename_for_stat, buffer, sizeof (buffer))) == -1)
	{
		perror("readlink");
		return NO_ERR;
	}
	else
	{
		buffer[link_string_length] = '\0';
		
		if (MOD_ALL(mod))
		{
			
			if (lstat(file_path, &file_stat) != 0) return GET_STAT_ERR;
			printf();
		}
		else
		{
			printf("%s -> %s\n", your_filename_for_stat, buffer);
		}
	
	}

	return NO_ERR;
}