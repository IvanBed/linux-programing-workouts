#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "store_ll.h"

static int exit_signal = 0;
static pthread_mutex_t m;
static pthread_cond_t cond;

void* get_data(void *data) {

    struct Store *store = (struct Store*) data;
    
    while (!exit_signal) {
        pthread_mutex_lock(&m);
        while (store->size < 0)
            pthread_cond_wait(&cond, &m);
            

        int res = 0;
        get_val2(&store, &res);
         
        printf("the value: %d has gotten. \n", res);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
		sleep(2);
	}

	
}


void* set_data(void *data) {

    struct Store *store = (struct Store*) data;
    
    while (!exit_signal) {
        pthread_mutex_lock(&m);
        while (store->size >= store->capacity)
			pthread_cond_wait(&cond, &m);
  
        add_val(&store, 2);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&m);
		sleep(2);
   }

}


int main(int argc, char** argv) {
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_t thread_setter, thread_getter;
	struct Store *store_inst = NULL;
	store_init(&store_inst, 10);

    if (pthread_create(&thread_setter, NULL, get_data, store_inst) != 0) {
        puts("Could not start setter thread");
        return 1;
    }

    if (pthread_create(&thread_getter, NULL, set_data, store_inst) != 0) { 
        puts("Could not start setter thread");
        return 1;
    }
 
    pthread_join(thread_setter, NULL);
    pthread_join(thread_getter, NULL);

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cond);

    return 0;
}
