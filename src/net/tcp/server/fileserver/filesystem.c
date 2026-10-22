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
        return NULL;
    }

    file_info->file_size = get_file_size(file_info->file);
    sprintf(file_info->file_size_str, "%ld\n", file_info->file_size);
    return file_info;
}

FileInfo *create_file(char const * file_path)
{
    FileInfo *file_info = (FileInfo*)malloc(sizeof(FileInfo));
    memset(file_info, 0, sizeof(FileInfo));
    
    file_info->file = fopen(file_path, "wb");
    if (!file_info->file)
    {
        return NULL;
    }
    sprintf(file_info->file_size_str, "%ld\n", 0);
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
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
        return false;
}

void create_dir(char const *full_path)
{
    if (!full_path)
    {
        return;
    }
    int result = mkdir(full_path, 0755);
}

void copy_file(char const * dest_file, char const * src_file)
{
    FILE *src_f;
    FILE *dest_f;
    char file_buf[PAGESIZE];
    size_t index = 0;

    src_f = fopen(src_file, "r");
    if (!src_f)
    {
        printf("Could not open source file");
        return;
    }

    dest_f = fopen(dest_file, "w");
    if (!dest_f)
    {
        printf("Could not open destination file");
        fclose(src_f);
        return;
    }

    memset(file_buf, 0, PAGESIZE);

    while (!feof(src_f))
    {
        if (ferror(src_f))
        {
            printf("File Error\n");
            break;
        }
        file_buf[index++] = (char)fgetc(src_f);
        if (index == PAGESIZE)
        {
            fwrite(file_buf, sizeof(char), PAGESIZE, dest_f);
            index = 0;
            memset(file_buf, 0, PAGESIZE);
        }
    }

    if (index > 1)
    {
        fwrite(file_buf, sizeof(char),  index - 1, dest_f);
    }

    fclose(src_f);
    fclose(dest_f);
}

void copy_file_new(char const * dest_file, char const * src_file)
{
    FILE *src_f;
    FILE *dest_f;
    char file_buf[PAGESIZE];
    size_t read_cnt = 0;

    src_f = fopen(src_file, "r");
    if (!src_f)
    {
        printf("Could not open source file");
        return;
    }

    dest_f = fopen(dest_file, "w");
    if (!dest_f)
    {
        printf("Could not open destination file");
        fclose(src_f);
        return;
    }

    memset(file_buf, 0, PAGESIZE);

    while(!feof(src_f)) 
    {
        if (ferror(src_f)) 
        {
            printf("File Error\n");
            break;
        }
        read_cnt = fread(file_buf, sizeof(char), PAGESIZE, src_f);
        fwrite(file_buf, sizeof(char), read_cnt, dest_f);  
        //memset(file_buf, 0, PAGESIZE); 
    } 
    fclose(src_f);
    fclose(dest_f);
}

void delete_file(char const *file_name)
{
    if (remove(file_name) == 0)
        printf("File deleted successfully.\n");
    else
        printf("Error: Unable to delete the file.\n");

}

void move_file(char const * dest_dir_path, char const * src_dir_path, char const *file_name)
{
    char new_file_path[PATH_SIZE];
    char old_file_path[PATH_SIZE];

    snprintf(new_file_path, PATH_SIZE, "%s/%s\0", dest_dir_path, file_name);
    snprintf(old_file_path, PATH_SIZE, "%s/%s\0", src_dir_path, file_name);

    printf("new file %s\n", new_file_path);
    printf("old file %s\n", old_file_path);

    copy_file(new_file_path, old_file_path);
    delete_file(old_file_path);
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
        if (strncmp(dir_entry->d_name, pattern, char_cnt) == 0)
        {
            move_file(dest_dir_path, src_dir_path, dir_entry->d_name);
        }
    }

    closedir(dir);
}


