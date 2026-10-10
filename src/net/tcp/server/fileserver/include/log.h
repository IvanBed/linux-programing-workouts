#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdatomic.h>

#define DEFAULT_MESSAGE_SIZE 4096

typedef enum  
{
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_RED,
} log_level;

typedef struct message_desc
{
    char                 message[DEFAULT_MESSAGE_SIZE];
    int                  error_code;
    log_level            level;
    time_t               message_time;
    
} message_desc;

typedef struct queue_node
{
    struct queue_node   *next;  
    struct queue_node   *prev;

    message_desc         msg_desc;
} queue_node;

typedef struct log_message_queue 
{
    queue_node     *head;
    queue_node     *tail;
    
    size_t          size;
    size_t          capacity;
    
    
    pthread_mutex_t mtx;
    pthread_cond_t  cond;
  
    bool            critical_size;  
                        
} log_message_queue;

typedef struct log_desc 
{
    log_message_queue *log_queue;
    FILE               file_desc;
    
} log_desc;

#endif