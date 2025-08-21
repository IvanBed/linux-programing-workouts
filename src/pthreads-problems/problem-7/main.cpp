#include <stdio.h>
#include <pthread.h>
#include "store_ll.h"


static phthread_mutex_t m;
static pthread_cond_t cond;
static int cond_predicate;

void* get_data(void *data) {

    struct Store *store = (sturct Store*) data;
    
    while (1) {
        phtread_mutex_lock(&m, NULL);
        while (cond_predicte != 1)
            pthread_condition_wait(&cond, &m);
            cond_predicate == 0;
        if (store->size == 0) {
             pthread_condition_signal(&cond, &m);
             pthread_mutex_unlock(&m, NULL);
             cond_predicate = 1;
             sleep(10);
        } else {
             int res = 0;
             get_value(&(*store->head), &res);
             store->size--;
             std::cout << "the value: " << res << " has gotten. \n";
             pthread_condition_signal(&cond, &m);
             cond_predicate = 1;
             pthread_mutex_unlock(&m, NULL);
        }

    }

}


void* set_data() {

 struct Store *store = (sturct Store*) data;
    
    while (1) {
        phtread_mutex_lock(&m, NULL);
        while (cond_predicte != 1)
            pthread_condition_wait(&cond, &m);
       
        if (store->size >= store->capacity) {
            pthread_condition_signal(&cond, &m);
            pthread_mutex_unlock(&m, NULL);
            cond_predicate = 1;
            sleep(10);
        } else {
            add_val(&(*store->head), 2);
             store->size++;
             pthread_condition_signal(&cond, &m);
             cond_predicate = 1;
             pthread_mutex_unlock(&m, NULL);

       }
 
   }

}


int main(int argc, char** argv) {
    pthread_mutex_init(&m);
    pthread_condition_init(&cond);
    pthread_t thread_setter, thread_getter;

    if (pthread_create(thread_setter, NULL, get_data, store) != 0) {
        std::cout << "Could not start setter thread\n";
        return 1;
    }

    if (pthread_create(thread_getter, NULL, set_data, store) != 0) { 
        std::cout << "Could not start setter thread\n";
        return 1;
    }

    pthread_join(thread_setter, NULL);
    pthread_join(thread_getter, NULL);

    pthread_mutex_destroy(&m);
    pthread_condition_destroy(&cond);

    return 0;
}
