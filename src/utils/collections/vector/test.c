#include "number_interface.h"

void test_int()
{
    vector *test = NULL;
    vector_init(1, sizeof(int64_t),&test);
    add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
    add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	add_int(test, 12);
    add_int(test, 13);
    add_int(test, 14);
    add_int(test, 15);
    add_int(test, 16);
    add_int(test, 17);
    add_int(test, 19);
    add_int(test, 123);
	
    for (size_t i = 0; i < test->size; i++)
    {
        printf("%d\n", get_int(test, i));        
    }
    printf("size: %d\n", test->size);
    //int_vector_destroy(test);
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
    str_vector_init(20, &test);
    char *t1 = "aaa";
    char *t2 = "bbb";
    char *t3 = "ccc";
    add_string(test, t1);
    add_string(test, t2);
    add_string(test, t3);
    for (size_t i = 0; i < test->size; i++)
    {
        printf("%s\n", get_string(test, i));        
    }
    printf("size: %d\n", test->size);
    str_vector_destroy(test);
}

int main() 
{
    test_int();
    test_double();
    //test_str();
}