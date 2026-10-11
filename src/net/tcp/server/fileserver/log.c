#include "include/log.h"

log_desc *create_log(char const *log_dir)
{
    //check_log_file
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

    check_log_dir(log_dir)


    
}

void destroy_log(log_desc * log_inst)
{


}

void *log_writer_routine(void* data)
{
    log_desc *ld = data;
    message_desc *msg_desc = malloc(sizeof(message_desc));

    while(true)
    {
        memset(msg_desc, 0, sizeof(message_desc));
        pop_message(ld->log_queue, msg_desc);
        write_file(ld->file_desc, msg_desc);
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
