#ifndef FILESYSTEM_H

#define FILESYSTEM_H
#define ENOUGH 64

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct FileInfo 
{
    FILE *file;
    size_t file_size;
    char   file_size_str[ENOUGH];
} FileInfo;

FileInfo *open_file(char const * file_path);
void close_file(FileInfo *file_info);

#endif