#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <signal.h>

static atomic_int exit_signal = 0;
static sem_t a_sem;
static sem_t b_sem;
static sem_t widget_sem_1;
static sem_t widget_sem_2;

void sigint_handler(int sig) {
    puts("SIGINT signal recived. Terminated...");
    exit_signal = 1;
    puts("Flag exit_signal is TRUE.");
}

static void* makeA(void* data) {
	while(!exit_signal) {
    	sleep(1);
		puts("A is ready!");
    	sem_post(&a_sem);
	}
	puts("Thread makeA is terminated.");
	pthread_exit(NULL);
}

static void* makeB(void* data) {
	while(!exit_signal) {
        sleep(2);
		
    	puts("B is ready!");
		sem_post(&b_sem);
	}
	puts("Thread makeB is terminated.");
	pthread_exit(NULL);
}

static void* makeAB(void* data) {
	while(!exit_signal) {
		sem_wait(&a_sem);
		sem_wait(&b_sem);
    	puts("AB is ready!");
		sem_post(&widget_sem_1);
	}
	puts("Thread makeAB is terminated.");
	pthread_exit(NULL);
}

static void* makeC(void* data) {
    while(!exit_signal) {
    	sleep(3);
    	puts("C is ready!");
    	sem_post(&widget_sem_2); 
    }
	puts("Thread makeC is terminated.");
	pthread_exit(NULL);
}

static void* makeWidget(void* data) {
	while(!exit_signal) {
		sem_wait(&widget_sem_1);
		sem_wait(&widget_sem_2);
    	puts("Widget is ready!");
	}
	puts("Thread makeWidget is terminated.");
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);
	pthread_t thread_a, thread_b, thread_ab, thread_c, thread_widget;
		
	if (sem_init(&a_sem, 0, 0) != 0) {
		perror("Unable to init a_sem");
	}
	
	if (sem_init(&b_sem, 0, 0) != 0) {
		perror("Unable to init b_sem");
	}
	
	if (sem_init(&widget_sem_1, 0, 0) != 0) {
		perror("Unable to init widget_sem_1");
	}
	
	if (sem_init(&widget_sem_2, 0, 0) != 0) {
		perror("Unable to init widget_sem_2");
	}

	if (pthread_create(&thread_a, NULL, makeA, NULL) != 0) {
		perror("Unable to create thread for function makeA.");
	}
	
	if (pthread_create(&thread_b, NULL, makeB, NULL) != 0) {
		perror("Unable to create thread for function makeB.");
	}
	
	if (pthread_create(&thread_ab, NULL, makeAB, NULL) != 0) {
		perror("Unable to create thread for function makeAB.");
	}

	if (pthread_create(&thread_c, NULL, makeC, NULL) != 0) {
		perror("Unable to create thread for function makeC.");
	}
	
	if (pthread_create(&thread_widget, NULL, makeWidget, NULL) != 0) {
		perror("Unable to create thread for function makeWidget.");
	}
	
	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);
	pthread_join(thread_ab, NULL);
	pthread_join(thread_c, NULL);
	pthread_join(thread_widget, NULL);
	
	puts("All threads are terminated.");
	
	if (sem_destroy(&a_sem) != 0) {
		perror("Unable to destroy a_sem.");
	}
	
	if (sem_destroy(&b_sem) != 0) {
		perror("Unable to destroy b_sem.");
	}
	
	if (sem_destroy(&widget_sem_1) != 0) {
		perror("Unable to destroy widget_sem_1.");
	}
	if (sem_destroy(&widget_sem_2) != 0) {
		perror("Unable to destroy widget_sem_2.");
	}
	
	return 0;
	
}