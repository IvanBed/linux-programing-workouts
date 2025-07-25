#include <stdio.h>                                                              
#include <stdlib.h>
#include <pthread.h>                                                            
#include <limits.h>                                                            
#include <errno.h>     
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

// Решение таска 13.

static sem_t sem1; 
static sem_t sem2;

static int create_stack(void** mystack, size_t* mystacksize) {
    *mystacksize = 2 * PTHREAD_STACK_MIN;   
    *mystack = aligned_alloc(PTHREAD_STACK_MIN, *mystacksize);
    
    if (*mystack == NULL) {
        perror("Unable to acquire storage");
        return 1;
    }

    printf("Setting stackaddr to %p\n", *mystack);
    printf("Setting stacksize to %zu\n", *mystacksize);

    return 0;
}

static void* print_lines(void* data) {
    
	int line_num = 1;
    pthread_t id =  pthread_self(); 
    struct sched_param thred_param;
    int policy = 0;
    int s = pthread_getschedparam(id, &policy, &thred_param);
    if (s != 0)
        printf("Problems");
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_setcancelstate");
    
    for (size_t i = 0; i < 10; i++) {
        sem_wait(&sem1);
		printf("%lu thread line # %d, priority is %d\n", id, line_num++, thred_param.sched_priority);
		sem_post(&sem2); 
	}
    printf("line_num pointer points ----> %p\n", &line_num);
  
    return 0;
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    pthread_t thread1;
    pthread_attr_t attr;
		
	sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);
	
    void *res;
 
    void* mystack;                                                                            
    size_t stacksize;
 
    int err = create_stack(&mystack, &stacksize);
    if (err != 0) {
        pthread_attr_destroy(&attr);
        exit(2);
    }
        
    pthread_attr_init(&attr);
    
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    struct sched_param param;
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = 3;
    pthread_attr_setschedparam(&attr, &param);    
    
    pthread_create(&thread1, &attr, print_lines, NULL);    
    
    int line_num = 1;
    for (size_t i = 0; i < 10; i++) {
        sem_wait(&sem2);
		printf("Main thread line # %d\n", line_num++);
		sem_post(&sem1); 
		
	}
        
    
    printf("Main thread line_num pointer points ----> %p\n", &line_num);
    
    int s = pthread_join(thread1, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_join");

    if (res == PTHREAD_CANCELED)
        printf("%s(): thread was canceled\n", __func__);
    else
        printf("%s(): thread wasn't canceled (shouldn't happen!)\n",
            __func__);
          
    
    free(mystack);   
    pthread_attr_destroy(&attr);   
    sem_destroy(&sem1);  
    sem_destroy(&sem2);  
    exit(EXIT_SUCCESS);
}
