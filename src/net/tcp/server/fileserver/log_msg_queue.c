#include "include/log_msg_queue.h"

static queue_node *new_node(message_desc const *msg_desc)
{
    queue_node * node = malloc(sizeof(queue_node));
    if (!node)
    {
        return NULL;
    }
    
    node->next = NULL;
    node->prev = NULL;
    
    memcpy(&node->msg_desc, msg_desc, sizeof(message_desc));
    return node;
}

static void free_node(queue_node *node)
{
    free(node);
}

log_message_queue *create_log_queue(size_t max_size) 
{
    log_message_queue * log_queue = malloc(sizeof(log_message_queue));
    
    log_queue->head = NULL;
    log_queue->tail = NULL;
    log_queue->size = 0;
    log_queue->capacity = max_size;
    log_queue->critical_size = true; 
    
    if (pthread_mutex_init(&log_queue->mtx, NULL) !=  0)
    {
        free(log_queue);
        return NULL;
    }
    
    if (pthread_cond_init(&log_queue->cond, NULL) !=  0)
    {
        pthread_mutex_destroy(&log_queue->mtx);
        free(log_queue);
        return NULL;
    }
    return log_queue;
}

void destroy_log_queue(log_message_queue *log_queue)
{
    if (!log_queue)
    {
        return;
    }
    
    queue_node *temp    = NULL;
    queue_node *current = log_queue->tail;
    
    pthread_mutex_lock(&log_queue->mtx);
    
    while(current)
    {
        temp = current;
        current = current->next;
        free(temp);
    }

    pthread_mutex_unlock(&log_queue->mtx);

    pthread_mutex_destroy(&log_queue->mtx);
    pthread_cond_destroy(&log_queue->cond);
    free(log_queue);
}

static void add_tail(log_message_queue *log_queue, queue_node *node)
{
    if (!log_queue || !node)
    {
        return;
    }
    
    if (log_queue->size == 0) 
    {
        log_queue->head = node;
        log_queue->tail = node;
        
    } else 
    {
        node->next = log_queue->tail;
        log_queue->tail->prev = node;
        log_queue->tail = node;
    }
    log_queue->size++;
}

static void delete_head(log_message_queue *log_queue)
{
    if (!log_queue)
    {
        return;
    }  
  
    if (log_queue->size == 0)
    {
        return;
    }

    queue_node *temp = NULL;
    
    temp = log_queue->head;

    if (log_queue->head == log_queue->tail)
    {
        log_queue->head = NULL;
        log_queue->tail = NULL;
    } else 
    {
        log_queue->head = log_queue->head->prev;
        log_queue->head->next = NULL;
    }
    
    log_queue->size--;
    free_node(temp);
}


void push_message(log_message_queue *log_queue, message_desc const *msg_desc)
{
    if (!log_queue || !msg_desc)
    {
        return;
    }
    
    queue_node *node = new_node(msg_desc);
    
    pthread_mutex_lock(&log_queue->mtx);
    add_tail(log_queue, node);
    
    //if (log_queue->size > 2)
    //    log_queue
    
    pthread_cond_signal(&log_queue->cond);
    pthread_mutex_unlock(&log_queue->mtx);
}

void pop_message(log_message_queue *log_queue, message_desc *local_msg_desc)
{
    pthread_mutex_lock(&log_queue->mtx);
    while(log_queue->size == 0)
        pthread_cond_wait(&log_queue->cond, &log_queue->mtx);
    
    // copy data from the queue to the local worker local mem
    memcpy(local_msg_desc, &(log_queue->head->msg_desc), sizeof(message_desc));
    
    delete_head(log_queue);
    pthread_mutex_unlock(&log_queue->mtx);
}

static void print_queue(log_message_queue *log_queue)
{
    queue_node *node = log_queue->tail;
    
    while(node)
    {
        printf("%s ", node->msg_desc.message);
        node = node->next;
    }
}
