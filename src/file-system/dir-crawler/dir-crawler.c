#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#define NO_ERR 0
#define DIR_OPD_ERR 1
#define GET_STAT_ERR 2
#define FILES_STAT_INIT_ERR 3
#define EMPTY_DIR_PATH 4

typedef struct files_stat files_stat;

struct files_stat
{
    size_t count_file; 
    size_t count_not_permitted;
};

uint8_t file_stat_init(files_stat *f_stat)
{
    f_stat->count_file = 1; //count root dir
    f_stat->count_not_permitted = 0;
    return 0;
}

char const *get_type_linux(unsigned char  d_type)
{
    switch(d_type)
    {
        case DT_DIR: 
            return "It is a directory";
        case DT_BLK: 
            return "It is a block";
        case DT_REG: 
            return "It is a file";
        case DT_SOCK: 
            return "It is a socket";
        case DT_LNK: 
            return "It is a link";
        case DT_FIFO: 
            return "It is a named pipe";
        default:
            return "Unknown type";
    } 
}

//Решить проблему с лишним /

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

// Разобраться с функией stat, почему ошибка, какая ошибка? check errno

uint8_t dfs_dir_tree(char const *dir_path, uid_t user_id)
{
    if (dir_path == NULL || dir_path == "")
        return EMPTY_DIR_PATH;

    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("Could not open directory: %s, please check syntex correctness\n", dir_path);
        return DIR_OPD_ERR;
    }

    struct       dirent *dir_entry   = NULL; 
    char const  *next_file_path      = NULL;
    struct       stat file_stat;
    uint8_t      err                 = NO_ERR;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) continue;

        next_file_path = make_absolute_path(dir_entry->d_name, dir_path);
        if (lstat(next_file_path, &file_stat) != 0) return GET_STAT_ERR; 
        printf("%s ",  dir_entry->d_name);
        if (S_ISDIR(file_stat.st_mode))
        {
            if (file_stat.st_uid == user_id)
            {
                err = dfs_dir_tree(next_file_path, user_id);
                if (err != 0) 
                {
                    perror("dfs_dir_tree: ");
                    printf("\n");
                    free(next_file_path);
                    continue;
                }
            }   
            else
            {
                printf("Permission denied!\n");
            }
                
        }
        free(next_file_path);
    }

    printf("\n");
    closedir(dir);
    
    return NO_ERR;
}

uint8_t count_dir_entities_linux(char const *dir_path, files_stat *f_stat)
{
    if (dir_path == NULL || dir_path == "")
        return EMPTY_DIR_PATH;

    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        (f_stat->count_not_permitted)++;
        printf("Could not open directory: %s\n", dir_path);
        return DIR_OPD_ERR;
    }

    struct       dirent *dir_entry   = NULL; 
    char const  *next_file_path      = NULL;
    uint8_t      err                 = NO_ERR;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) continue;
        f_stat->count_file = f_stat->count_file + 1;
        next_file_path = make_absolute_path(dir_entry->d_name, dir_path);
        if (dir_entry->d_type == DT_DIR)
        {
            err = count_dir_entities_linux(next_file_path, f_stat);
            if (err != 0) 
            {
                free(next_file_path);
                continue;
            }
        }   
        free(next_file_path);
    }
    closedir(dir);
    
    return NO_ERR;
}

int main(int argc, char** argv) 
{
    
    if (argc < 2)
    {
        printf("Please, set up the start directory!\n");
        return 1;
    }

    uid_t   user_id      = getuid();
    uint8_t exec_code    = NO_ERR;
	
	files_stat f_sts;
    if (file_stat_init(&f_sts) != 0)
    {
        exec_code = FILES_STAT_INIT_ERR;
        return exec_code; 
    }
    
    exec_code = count_dir_entities_linux(argv[1], &f_sts);
    printf("Results:\nTotal file count: %ld\nTotal not permitted files: %ld\n", f_sts.count_file, f_sts.count_not_permitted);
    printf("Code of execution: %d\n", exec_code);
   
    return exec_code;
}



