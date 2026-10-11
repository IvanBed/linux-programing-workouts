#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "include/log_msg_queue.h"

typedef struct log_desc 
{
    log_message_queue *log_queue;
    FILE               file_desc;
    
} log_desc;

#endif