#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "store_ll.h"
#include "rand.h"

static int exit_signal = 0;
static pthread_mutex_t m;
static pthread_cond_t cond;

static void* get_data(void *data) {
    struct Store *store = (struct Store*) data;
	int res = 0;
    while (!exit_signal) {
        pthread_mutex_lock(&m);
        while (store->size < 0)
            pthread_cond_wait(&cond, &m);

		if (get_val(&store, &res) != 0) {
			fprintf(stderr, "Could not get a value from the store!\n");
		}
		
        printf("the value: %d has gotten. \n", res);
       
		pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
		
		sleep(get_random_real(0.5, 2));
	}
}


static void* set_data(void *data) {
    struct Store *store = (struct Store*) data;
    while (!exit_signal) {
        pthread_mutex_lock(&m);
		while (store->size >= store->capacity)
			pthread_cond_wait(&cond, &m);
  
        add_val(&store, 2);
		
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
		
		sleep(get_random_real(0.5, 2));
   }
}


int main(int argc, char** argv) {
	
	pthread_t thread_setter, thread_getter;
	struct Store *store_inst = NULL;
	
	if (pthread_mutex_init(&m, NULL) != 0) {
		fprintf(stderr, "Could not init the mutex!\n");
	}
	
    if (pthread_cond_init(&cond, NULL) != 0) {
		fprintf(stderr, "Could not init the condition!\n");
	}
    
	if (store_init(&store_inst, 10) != 0) {
		fprintf(stderr, "Could not init the store!\n");
		return 1;
	}

    if (pthread_create(&thread_setter, NULL, get_data, store_inst) != 0) {
        fprintf(stderr, "Could not start setter thread\n");
        return 1;
    }

    if (pthread_create(&thread_getter, NULL, set_data, store_inst) != 0) { 
        fprintf(stderr, "Could not start setter thread");
        return 1;
    }
 
    pthread_join(thread_setter, NULL);
    pthread_join(thread_getter, NULL);

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cond);

    return 0;
}
