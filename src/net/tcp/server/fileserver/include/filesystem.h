#ifndef FILESYSTEM_H

#define FILESYSTEM_H
#define ENOUGH 64

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_SIZE 4096
#define PAGESIZE 4096

#define NO_ERR 0
#define DIR_OPEN_ERR 1

typedef struct FileInfo 
{
    FILE  *file;
    size_t file_size;
    char   file_size_str[ENOUGH];
} FileInfo;

FileInfo *open_file(char const * file_path);
FileInfo *create_file(char const * file_path);
void close_file(FileInfo *file_info);

bool check_dir(char const *dir_path);
void create_dir(char const *full_path);
void move_files_to_dir(char const *src_dir_path, char const *dest_dir_path, char const *pattern, size_t char_cnt);

#endif