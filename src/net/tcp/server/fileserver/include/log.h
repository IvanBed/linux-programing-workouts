#ifndef LOG_H
#define LOG_H

#include "log_msg_queue.h"
#include "filesystem.h"

#define WORK 0
#define SHUTDOWN 1
#define LOG_QUEUE_SIZE

#define LOG_WRITE(log_desc, msg) log_write(log_desc, msg, 0, 0);
#define LOG_ERROR(log_desc, msg) log_write(log_desc, msg, 1, 1);

typedef struct LogDesc 
{
    log_message_queue *log_queue;
    FILE              *file_desc;
    atomic_int         shutdown_flag;
    
} LogDesc;

typedef struct LogThreadWrapper 
{
    LogDesc   *log_desc;
    pthread_t  tid;  
} LogThreadWrapper;

LogDesc *create_log(char const *log_dir, size_t log_queue_size);
void destroy_log(LogDesc * log_inst);
void *log_writer_routine(void* data);
void log_write(LogDesc *ld, char * const message, int error, log_level level);

#endif