#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "include/log_msg_queue.h"
#include "include/filesystem.h"

#define WORK 0
#define SHUTDOWN 1
#define LOG_QUEUE_SIZE

typedef struct log_desc 
{
    log_message_queue *log_queue;
    FILE              *file_desc;
    atomic_int         shutdown_flag;
    
} log_desc;

#endif