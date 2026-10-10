#include "include/log.h"


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

static log_message_queue *create_log_queue(size_t max_size) 
{
    log_message_queue * log_queue = malloc(sizeof(log_message_queue));
    
    log_queue->head = NULL;
    log_queue->tail = NULL;
    log_queue->size = 0;
    log_queue->capacity = max_size;
    log_queue->critical_size = ATOMIC_VAR_INIT(true);; 
    
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

static void destroy_log_queue(log_message_queue *log_queue)
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

log_desc *create_log()
{
    //check_log_file
    log_desc * l = malloc(sizeof(log_desc));
    log_message_queue * lq = create_log_queue(1024); 

    


}

void destroy_log(log_desc * log_inst)
{


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


static void push_message(log_message_queue *log_queue, message_desc const *msg_desc)
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

static void pop_message(log_message_queue *log_queue, message_desc *local_msg_desc)
{
    pthread_mutex_lock(&log_queue->mtx);
    while(log_queue->size == 0)
        pthread_cond_wait(&log_queue->cond, &log_queue->mtx);
    
    // copy data from the queue to the local worker local mem
    memcpy(local_msg_desc, &(log_queue->head->msg_desc), sizeof(message_desc));
    
    delete_head(log_queue);
    pthread_mutex_unlock(&log_queue->mtx);
}

static int write_file(FILE file_desc, message_desc *msg_desc)
{


    return 0;
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

static void print_queue(log_message_queue *log_queue)
{
    queue_node *node = log_queue->tail;
    
    while(node)
    {
        printf("%s ", node->msg_desc.message);
        node = node->next;
    }
    
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
