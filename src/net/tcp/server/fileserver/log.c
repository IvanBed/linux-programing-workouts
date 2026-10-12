#include "include/log.h"

void arch_logs(char const *log_dir)
{
    char        arch_dir[PATH_SIZE];
    char const *pattern = "fileserver.log";

    snprintf(arch_dir, PATH_SIZE, "%s/%s\0", arch_dir, "arch");

    if (!check_dir(arch_dir))
    {
        create_dir(arch_dir);
    }

    move_files_to_dir(log_dir, arch_dir, pattern, strlen(pattern));

}

log_desc *create_log(char const *log_dir)
{
    FILE *log_file;
    char log_name[PATH_SIZE];

    log_desc * l = malloc(sizeof(log_desc));
    if (!l)
    {
        printf("Could not allocate memoru fo log desc\n");
        return;
    }
    
    log_message_queue * lq = create_log_queue(1024); 
    if (!lq)
    {
        printf("Could not allocate memoru fo log message queue\n");
        free(l);
        return;
    }

    arch_logs(log_dir);

    snprintf(log_name, PATH_SIZE, "%s/s.%ld\0", log_dir, "fileserver.log", (unsigned long)time(NULL));
    log_file = fopen(log_name, "w");

    l->log_queue = lq;
    l->file_desc = log_file;
    l->shutdown_flag = WORK; 

    return l;

}

void destroy_log(log_desc * log_inst)
{
    close(log_inst->file_desc);
    destroy_log_queue(log_inst->log_queue);
    free(log_inst);

}

void write_to_logfile(FILE *file, message_desc *msg_desc)
{
    char file_buf[DEFAULT_MESSAGE_SIZE + 1024];
    size_t index = 0;


    char time_buf[17];
    struct tm info = *localtime(&msg_desc->message_time);
    sprintf(time_buf, "%02d/%02d/%4d %02d:%02d", info.tm_mon + 1, info.tm_mday, 1900 + info.tm_year,
           info.tm_hour, info.tm_min);

    
    sprintf(file_buf, "thread id: %ld %s %s %ld %ld", msg_desc->tid, time_buf, msg_desc->message, msg_desc->error_code, msg_desc->error_code);
}

void *log_writer_routine(void* data)
{
    log_desc *ld = data;
    message_desc *msg_desc = malloc(sizeof(message_desc));

    while(!ld->shutdown_flag)
    {
        memset(msg_desc, 0, sizeof(message_desc));
        pop_message(ld->log_queue, msg_desc);
        write_to_logfile(ld->file_desc, msg_desc);
    }
    free(msg_desc);
}

void log_write(log_desc *ld, char * const message, int error, log_level level)
{
    time_t current_timestamp = time(NULL);
    message_desc msg_desc;

    memcpy(&msg_desc.message, message, DEFAULT_MESSAGE_SIZE);
    msg_desc.error_code = error;
    msg_desc.level = level;
    msg_desc.message_time = current_timestamp;

    pop_message(ld->log_queue, &msg_desc);
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
