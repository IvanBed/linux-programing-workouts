#include<stdio.h>
#include<pthread.h>

// 4, 9
//pthread_cleanup_push

pthread_mutex_t m;

static void print_goodbye(void* arg) {
	
    pthread_t* thr_id_ptr = (pthread_t*)arg;
	printf("%s --- %lu\n", *thr_id_ptr);
	
}

static void* print_lines(void* data) {
	
	char** arr = (char**) data;
	for (size_t i = 0; i < 4; i++)
	    printf("%s --- %lu\n", arr[i], pthread_self());
	
	pthread_cleanup_push(&print_goodbye, (void*)(&pthread_self()));
	pthread_cleanup_pop(1);
	return NULL;
	
}
 

int main(int argc, char** argv) {
	
	pthread_mutex_init(&m, NULL);
	
	char* str_arr1[4] = {"a","b","c","d"};
    char* str_arr2[4] = {"e","r","t","y"};
    char* str_arr3[4] = {"z","x","c","v"};
    char* str_arr4[4] = {"a","s","d","f"};
    
    pthread_t thr1, thr2, thr3, thr4;
    
    pthread_create(&thr1, NULL, print_lines, (void*)str_arr1);
    pthread_create(&thr2, NULL, print_lines, (void*)str_arr2);
    pthread_create(&thr3, NULL, print_lines, (void*)str_arr3);
    pthread_create(&thr4, NULL, print_lines, (void*)str_arr4);
    
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);
    pthread_join(thr3, NULL);
    pthread_join(thr4, NULL);
    
	pthread_mutex_destroy(&m);
	
	return 0;
	
}