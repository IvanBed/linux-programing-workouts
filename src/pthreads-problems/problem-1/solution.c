#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

// Решение тасков 1, 2, 3, 5, 6, 10, 11, 12

static pthread_mutex_t m;
static pthread_cond_t cond;
static int cond_predictate = 0;

static int create_stack(void** mystack, size_t* mystacksize) {
    *mystacksize = 2 * PTHREAD_STACK_MIN;
    *mystack = aligned_alloc(PTHREAD_STACK_MIN, *mystacksize);
    
    if (*mystack == NULL) {
        handle_error_en(1, "Unable to acquire storage.\n");
        return 1;
    }

    printf("Setting stackaddr to %p\n", *mystack);
    printf("Setting stacksize to %zu\n", *mystacksize);

    return 0;
}

static void* print_lines(void* data) {
    int line_num = 1;
    pthread_t id = pthread_self();
    struct sched_param thred_param;
    int policy = 0;
    
    int s = pthread_getschedparam(id, &policy, &thred_param);
    if (s != 0)
        handle_error_en(s, "Problem with pthread_getschedparam.\n");
    
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0)
        handle_error_en(s, "Problem with pthread_setcancelstate.\n");

    for (size_t i = 0; i < 10; i++) {
        pthread_mutex_lock(&m);
        while (!cond_predictate)
            pthread_cond_wait(&cond, &m);
        
        printf("%lu thread line # %d, priority is %d\n", id, line_num++, thred_param.sched_priority);
        cond_predictate = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
    }
    
    printf("line_num pointer points ----> %p\n", &line_num);
    
    return 0;
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    pthread_t thread1;
    pthread_attr_t attr;
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cond, NULL);
    void *res = NULL;
 
    void *mystack;
    size_t stacksize;
 
    int s = create_stack(&mystack, &stacksize);
    if (s != 0) {
        handle_error_en(s, "Could not create stack.\n");
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&cond);
        exit(EXIT_FAILURE);
    }
        
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    struct sched_param param;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = 3;
    pthread_attr_setschedparam(&attr, &param);
    
    s = pthread_create(&thread1, &attr, print_lines, NULL);
    if (s != 0) {
        handle_error_en(s, "Could not create thread.\n");
        free(mystack);
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&cond);
        exit(EXIT_FAILURE);
    }
    
    int line_num = 1;
    for (size_t i = 0; i < 10; i++) {
        pthread_mutex_lock(&m);
        while (cond_predictate)
            pthread_cond_wait(&cond, &m);
        
        printf("Main thread line # %d\n", line_num++);
        cond_predictate = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
    }
    
    printf("Main thread line_num pointer points ----> %p\n", &line_num);
    
    s = pthread_join(thread1, &res);
    if (s != 0)
        handle_error_en(s, "Problem with pthread_join.\n");

    if (res == PTHREAD_CANCELED)
        printf("%s(): thread was canceled\n", __func__);
    else
        printf("%s(): thread terminated normally\n", __func__);
    
    free(mystack);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cond);
    
    exit(EXIT_SUCCESS);
}