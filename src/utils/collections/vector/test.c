#include "number_interface.h"
#include <pthread.h>

void test_int()
{
    vector *test = NULL;
    int_vector_init(1, &test);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    add_int(test, 12, false);
    add_int(test, 13, false);
    add_int(test, 14, false);
    add_int(test, 15, false);
    add_int(test, 16, false);
    add_int(test, 17, false);
    add_int(test, 19, false);
    add_int(test, 123, false);
    
    set_int(test, 0,10000, false);
    set_int(test, 1,10000, false);
    set_int(test, 2,10000, false);
    set_int(test, 3,10000, false);
    set_int(test, 4,10000, false);
    set_int(test, 5,10000, false);
    for (size_t i = 0; i < test->size; i++)
    {
        printf("%d\n", get_int(test, i, false));        
    }
    printf("size: %d\n", test->size);
    int_vector_destroy(test);
}

void test_double()
{
    vector *test = NULL;
    double_vector_init(1, &test);
    add_double(test, 12.5);
    add_double(test, 13.56756);
    add_double(test, 14.3425);
    add_double(test, 15.9876);
    add_double(test, 16.123);
    add_double(test, 17.2134);
    add_double(test, 19.6545);
    add_double(test, 123.87654);
    for (size_t i = 0; i < test->size; i++)
    {
        printf("%f\n", get_double(test, i));        
    }
    printf("size: %d\n", test->size);
    double_vector_destroy(test);
}

void test_str()
{
    vector *test = NULL;
    str_vector_init(15, &test);
    char *t1 = "aaa";
    char *t2 = "bbb";
    char *t3 = "ccc";
    char *t4 = malloc(4);
    strcpy(t4, t1);
    add_string(test, t1);
    add_string(test, t2);
    add_string(test, t3);
    add_string(test, t4);
    add_string(test, "sdfsd");
    add_string(test, "sdw11");
    add_string(test, "132rsdfasdfsdf");
    for (size_t i = 0; i < test->size; i++)
    {
        char *res1;
        get_string(test, i, res1);
        //printf("%s\n", res1);
    }
    printf("size: %d\n", test->size);
    free(t4);
    str_vector_destroy(test);
   
}

void *add_data(void *data)
{
    int64_t test = 1000;
    vector *store_inst = (vector *)data;
    while(1)
    {
        add_int(store_inst, ++test, true);
        sleep(1);
    }
    return NULL;
}

void *set_data(void *data)
{
    int64_t test = 0;
    vector *store_inst = (vector *)data;
    while(1)
    {
        set_int(store_inst, 0, ++test, true);
        sleep(1);
    }
    return NULL;
}

void *get_data(void *data)
{
    vector *store_inst = (vector *)data;
    int64_t prev_val = -1;
    size_t index = 0;
    while(1)
    {
        int64_t val = get_int(store_inst, index++, true);
        
        printf("GETTER 1 value: %d\n", val);
        printf("GETTER 1 prev val: %d\n", prev_val);
        prev_val = val;
        sleep(1);
    }

    return NULL;
}

void *get_data2(void *data)
{
    vector *store_inst = (vector *)data;
    while(1)
    {
        int64_t val = get_int(store_inst, 1, true);
        printf("GETTER 2 value: %d\n", val);
        sleep(1);
    }

    return NULL;
}

void *print_all(void *data)
{
    vector *store_inst = (vector *)data;
    while(1)
    {
        sleep(1);
        for (size_t i = 0; i < store_inst->size; i++)
        {
            int64_t val = get_int(store_inst, i, true);
            printf("%d ", val);
        }
        printf("\n");   
    }

    return NULL;
}

void test_multithread()
{
    vector *store_inst = NULL;
    int_vector_multithread_init(5, &store_inst);
    
    pthread_t thread_setter, thread_getter, thread_adder, thread_print_all;

    add_int(store_inst, 1, false);
    add_int(store_inst, 2, false);

    if (pthread_create(&thread_setter, NULL, set_data, (void *)store_inst) != 0) {
        fprintf(stderr, "Could not start setter thread!\n");
        return 1;
    }

    if (pthread_create(&thread_getter, NULL, get_data, (void *)store_inst) != 0) { 
        fprintf(stderr, "Could not start getter thread!\n");
        return 1;
    }
 
    if (pthread_create(&thread_adder, NULL, add_data, (void *)store_inst) != 0) { 
        fprintf(stderr, "Could not start getter thread!\n");
        return 1;
    }
 
    if (pthread_create(&thread_print_all, NULL, print_all, (void *)store_inst) != 0) { 
        fprintf(stderr, "Could not start thread_print_all!\n");
        return 1;
    }
    pthread_join(thread_setter, NULL);
    pthread_join(thread_getter, NULL);
    pthread_join(thread_adder, NULL);
	pthread_join(thread_print_all, NULL);

    int_vector_multithread_destroy(store_inst);
}

int main() 
{
    /*test_int();
    test_double();
    test_int();
    test_double();
    test_int();
    test_double();
    test_int();*/
    //test_double();

    test_str();
    test_str();
    test_str();
    //test_multithread();
}
