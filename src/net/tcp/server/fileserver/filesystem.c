#include "include/filesystem.h"

static size_t get_file_size(FILE *f)
{
    size_t size;

    fseek(f, 0, SEEK_END);
    size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

FileInfo *open_file(char const * file_path)
{
    FileInfo *file_info = (FileInfo*)malloc(sizeof(FileInfo));
    memset(file_info, 0, sizeof(FileInfo));
    
    file_info->file = fopen(file_path, "rb");
    if (!file_info->file)
    {
        return 0;
    }

    file_info->file_size = get_file_size(file_info->file);
    sprintf(file_info->file_size_str, "%d\n", file_info->file_size);
    return file_info;
}

void close_file(FileInfo *file_info)
{
    fclose(file_info->file);
    free(file_info);
}