#include "include/log.h"

static void arch_logs(char const *log_dir)
{
    char        arch_dir[PATH_SIZE];
    char const *pattern = "fileserver.log";

    snprintf(arch_dir, PATH_SIZE, "%s/%s\0", log_dir, "arch");
    puts(arch_dir);
    if (!check_dir(arch_dir))
    {
        create_dir(arch_dir);
    }

    move_files_to_dir(log_dir, arch_dir, pattern, strlen(pattern));

}

LogDesc *create_log(char const *log_dir, size_t log_queue_size)
{
    FILE *log_file;
    char log_name[PATH_SIZE];

    LogDesc *log_desc = malloc(sizeof(LogDesc));
    if (!log_desc)
    {
        printf("Could not allocate memoru fo log desc\n");
        return NULL;
    }
    
    log_message_queue * log_queue = create_log_queue(log_queue_size); 
    if (!log_queue)
    {
        printf("Could not allocate memoru fo log message queue\n");
        free(log_desc);
        return NULL;
    }

    arch_logs(log_dir);

    snprintf(log_name, PATH_SIZE, "%s/%s.%ld\0", log_dir, "fileserver.log", (unsigned long)time(NULL));
    log_file = fopen(log_name, "w");

    log_desc->log_queue = log_queue;
    log_desc->file_desc = log_file;
    log_desc->shutdown_flag = WORK; 

    return log_desc;
}

void destroy_log(LogDesc * log_desc)
{
    fclose(log_desc->file_desc);
    destroy_log_queue(log_desc->log_queue);
    free(log_desc);
}

static void write_to_logfile(FILE *file, message_desc *msg_desc)
{
    char file_buf[DEFAULT_MESSAGE_SIZE + DEFAULT_MESSAGE_METAINFO_SIZE];
    size_t index = 0;

    char time_buf[17];
    struct tm info = *localtime(&msg_desc->message_time);
    sprintf(time_buf, "%02d/%02d/%4d %02d:%02d", info.tm_mon + 1, info.tm_mday, 1900 + info.tm_year,
           info.tm_hour, info.tm_min);

    
    sprintf(file_buf, "thread id: %ld date: %s message: %s error code: %ld %ld\n", msg_desc->tid, time_buf, msg_desc->message, msg_desc->error_code, msg_desc->level);

    fputs(file_buf, file);
    fflush (file);
}

void *log_writer_routine(void* data)
{
    LogDesc *ld = data;
    message_desc *msg_desc = malloc(sizeof(message_desc));

    while(!ld->shutdown_flag)
    {
        memset(msg_desc, 0, sizeof(message_desc));
        pop_message(ld->log_queue, msg_desc);
        write_to_logfile(ld->file_desc, msg_desc);
    }
    free(msg_desc);
    pthread_exit()
}

void log_write(LogDesc *log_desc, char * const message, int error, log_level level)
{
    time_t current_timestamp = time(NULL);
    message_desc msg_desc;

    memcpy(&msg_desc.message, message, DEFAULT_MESSAGE_SIZE);
    msg_desc.error_code = error;
    msg_desc.level = level;
    msg_desc.message_time = current_timestamp;
    
    // temp info
    msg_desc.tid = 0;
    
    push_message(log_desc->log_queue, &msg_desc);
}

void log_write_thread(LogDesc *log_desc, pthread_t tid, char * const message, int error, log_level level)
{
    time_t current_timestamp = time(NULL);
    message_desc msg_desc;

    memcpy(&msg_desc.message, message, DEFAULT_MESSAGE_SIZE);
    msg_desc.error_code = error;
    msg_desc.level = level;
    msg_desc.message_time = current_timestamp;
    
    // temp info
    msg_desc.tid = tid;
    
    push_message(log_desc->log_queue, &msg_desc);
}
