#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h> 
#include <stdlib.h>
#include <unistd.h>


static int exit_signal = 0;
static sem_t a_sem;
static sem_t widget_sem_1;
static sem_t widget_sem_2;


static void* makeA(void* data) {
	while(!exit_signal) {
    	sleep(1);
    	sem_post(&a_sem);
	}
	return NULL;
}

static void* makeB(void* data) {
	while(!exit_signal) {
        sem_wait(&a_sem);
    	sleep(2);
    	puts("AB is ready!");
		sem_post(&widget_sem_1);
	}
	return NULL;
}


static void* makeC(void* data) {
    while(!exit_signal) {
    	sleep(3);
    	puts("C is ready!");
    	sem_post(&widget_sem_2); 
    }
	return NULL;
}

static void* makeWidget(void* data) {
	while(!exit_signal) {
		sem_wait(&widget_sem_1);
		sem_wait(&widget_sem_2);
    	puts("Widget is ready!");
	}
	return NULL;
}


int main(int argc, char** argv) {
	
	pthread_t thread_a, thread_b, thread_c, thread_widget;
		
	if (sem_init(&a_sem, 0, 0) != 0) {
		perror("Unable to init a_sem");
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
	if (pthread_create(&thread_a, NULL, makeB, NULL) != 0) {
		perror("Unable to create thread for function makeB.");
	}

	if (pthread_create(&thread_c, NULL, makeC, NULL) != 0) {
		perror("Unable to create thread for function makeC.");
	}
	if (pthread_create(&thread_widget, NULL, makeWidget, NULL) != 0) {
		perror("Unable to create thread for function makeWidget.");
	}

	pthread_join(thread_a, NULL);
	pthread_join(thread_b, NULL);
	pthread_join(thread_c, NULL);
	pthread_join(thread_widget, NULL);
	
	if (sem_destroy(&a_sem) != 0) {
	}
	if (sem_destroy(&widget_sem_1) != 0) {}
	if (sem_destroy(&widget_sem_2) != 0) {}
	
	return 0;
	
}