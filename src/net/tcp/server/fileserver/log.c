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

    LogDesc * l = malloc(sizeof(LogDesc));
    if (!l)
    {
        printf("Could not allocate memoru fo log desc\n");
        return NULL;
    }
    
    log_message_queue * lq = create_log_queue(log_queue_size); 
    if (!lq)
    {
        printf("Could not allocate memoru fo log message queue\n");
        free(l);
        return NULL;
    }

    arch_logs(log_dir);

    snprintf(log_name, PATH_SIZE, "%s/%s.%ld\0", log_dir, "fileserver.log", (unsigned long)time(NULL));
    log_file = fopen(log_name, "w");

    l->log_queue = lq;
    l->file_desc = log_file;
    l->shutdown_flag = WORK; 

    return l;

}

void destroy_log(LogDesc * log_inst)
{
    fclose(log_inst->file_desc);
    destroy_log_queue(log_inst->log_queue);
    free(log_inst);

}

static void write_to_logfile(FILE *file, message_desc *msg_desc)
{
    char file_buf[DEFAULT_MESSAGE_SIZE + DEFAULT_MESSAGE_METAINFO_SIZE];
    size_t index = 0;

    char time_buf[17];
    struct tm info = *localtime(&msg_desc->message_time);
    sprintf(time_buf, "%02d/%02d/%4d %02d:%02d", info.tm_mon + 1, info.tm_mday, 1900 + info.tm_year,
           info.tm_hour, info.tm_min);

    
    sprintf(file_buf, "thread id: %ld %s %s %ld %ld\n", msg_desc->tid, time_buf, msg_desc->message, msg_desc->error_code, msg_desc->error_code);

    fputs(file_buf, file);
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
}

void log_write(LogDesc *ld, char * const message, int error, log_level level)
{
    time_t current_timestamp = time(NULL);
    message_desc msg_desc;

    memcpy(&msg_desc.message, message, DEFAULT_MESSAGE_SIZE);
    msg_desc.error_code = error;
    msg_desc.level = level;
    msg_desc.message_time = current_timestamp;
    
    // temp info
    msg_desc.tid = 0;
    
    push_message(ld->log_queue, &msg_desc);
}

/*int main()
{
    log_message_queue *log_queue = create_log_queue(1024);
    
    add_tail(log_queue, new_node("1",1,1));
    add_tail(log_queue, new_node("2",1,1));
    add_tail(log_queue, new_node("3",1,1));
    delete_head(log_queue);
    delete_head(log_queue);
    delete_head(log_queue);
    
    print_queue(log_queue);

    destroy_log_queue(log_queue);
    return 0;
}*/
