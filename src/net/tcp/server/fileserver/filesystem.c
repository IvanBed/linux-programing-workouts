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
    sprintf(file_info->file_size_str, "%ld\n", file_info->file_size);
    return file_info;
}

void close_file(FileInfo *file_info)
{
    fclose(file_info->file);
    free(file_info);
}

bool check_dir(char const *dir_path)
{
    DIR* dir = opendir(dir_path);
    if (dir) {
        closedir(dir);
        return true;
    } 
    else
        return false;

}

void create_dir(char const *dir_path, char const *dir_name)
{
    if (!dir_path)
    {
        return;
    }

    if (!dir_name)
    {
        return;
    }  

    char full_path[PATH_SIZE];
    snprintf(full_path, sizeof(PATH_SIZE), "%s %s", dir_path, dir_name);
    
    int result = mkdir(full_path, 0666);
}

void move_files_to_dir(char const *src_dir_path, char const *dest_dir_path, char const *pattern, size_t char_cnt)
{

    DIR *dir = opendir(src_dir_path);
    if (dir == NULL)
    {
        printf("Could not open directory: %s, please check syntex correctness\n", src_dir_path);
        return;
    }    

    struct dirent *dir_entry = NULL;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmpn(dir_entry->d_name, pattern, char_cnt) == 0)
        {
            move_file(dest_dir_path, src_dir_path, dir_entry->d_name);
        }
    }
    
    closedir(dir);

}