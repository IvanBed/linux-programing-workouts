#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CHR 10000000
#define USER_DIR "/home/test/c_thread_app/"

static pthread_mutex_t m;
static pthread_cond_t cond;
static int is_mutex_init = 0;

struct file_decr {
    char* file_name;
    int* lock_ptr;
};

static int mutex_init() {
    if (is_mutex_init == 1)
        return 1;

    if (pthread_mutex_init(&m, NULL) != 0) {
        perror("pthread_mutex_init() error");
        return 1;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("pthread_cond_init() error");
        return 1;
    }
    is_mutex_init = 1;
    return 0;
}

static void mutex_destroy() {
    if (is_mutex_init == 0)
        return;
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cond);
    is_mutex_init = 0;
}

static char* get_cur_time_str() {
    time_t mytime = time(NULL);
    localtime(&mytime);
    char* time_frmd = ctime(&mytime);
    return time_frmd;
}

static int get_rand_number(int base, int user_start) {
    srand(user_start);
    return rand() % base;
}

static char* int_to_str(int num) {
    int length = snprintf(NULL, 0, "%d", num) + 1;
    char* str = malloc(length);
    if (!str) {
        perror("malloc failed");
        return NULL;
    }
    snprintf(str, length, "%d", num);
    return str;
}

static char* concat(char* str1, char* str2) {
    size_t new_str_len = strlen(str1) + strlen(str2);
    char* new_str = malloc(new_str_len + 1);
    if (new_str == NULL) {
        perror("Malloc failed");
        return NULL;
    }
    strcpy(new_str, str1);
    strcat(new_str, str2);
    return new_str;
}

static void* write_in_file(void* param) {
    struct file_decr* cur_file = (struct file_decr*)param;
    char* file_path = cur_file->file_name;
    size_t char_cnt = 0;
    int* res = malloc(sizeof(int));
	*res = 0;
    printf("File %s is opened for writing ", file_path);
    FILE* new_file = fopen(file_path, "w");
    if (new_file == NULL) {
        //perror("Error occurred while opening file");
        free(cur_file);
        *res = 1;
        return (void*)res;
    }

    size_t rand_size_block = get_rand_number(get_rand_number(100000, (int)pthread_self()), (int)pthread_self());

    for (size_t i = 0; i < rand_size_block; i++) {
        putc((char)get_rand_number(128, (int)i), new_file);
        char_cnt++;
    }

    printf("%zu chars were written to file %s\n", char_cnt, file_path);
    fclose(new_file);

    pthread_mutex_lock(&m);
    *(cur_file->lock_ptr) = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&m);

    free(cur_file);
    return (void*)res;
}

static void* read_file(void* param) {
    struct file_decr* cur_file = (struct file_decr*)param;
    char* file_path = cur_file->file_name;
    int* res = malloc(sizeof(int));
	*res = 0;
    pthread_mutex_lock(&m);
    while (*(cur_file->lock_ptr) != 1) {
        pthread_cond_wait(&cond, &m);
    }
    pthread_mutex_unlock(&m);

    printf("File %s is opened for reading ", file_path);
    size_t char_cnt = 0;
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        //perror("Error occurred while opening file");
        free(cur_file);
        *res = 1;
        return (void*) res;
    }

    while (fgetc(file) != EOF)
        char_cnt++;

    fclose(file);
    printf("%zu chars were read from file %s\n", char_cnt, file_path);

    free(cur_file);
    return (void*) res;
}

int main(int argc, char** argv) {
    size_t n = 0;

    if (argc != 2) {
        puts("Please provide n (must be even)");
        return 1;
    } else {
        n = atoi(argv[1]);
    }

    if (n % 2 != 0) {
        puts("N must be even");
        return 2;
    }

    if (mutex_init() != 0)
        return 3;

    char* file_store[n/2];
    pthread_t thread_store[n];
    int files_lock[n/2];
    memset(files_lock, 0, sizeof(files_lock));

    size_t thread_ptr = 0;
    char* dir_path = USER_DIR;

    // Create writer threads
    for (size_t i = 0; i < n/2; i++) {
        char* num_str = int_to_str(2*(i + 1) - 1);
        char* file_name = concat(dir_path, num_str);
        free(num_str);

        if (file_name == NULL) {
            continue;
        }

        file_store[i] = file_name;

        struct file_decr* cur_file = malloc(sizeof(struct file_decr));
        if (!cur_file) {
            perror("Failed to allocate file descriptor");
            free(file_name);
            continue;
        }

        cur_file->file_name = file_name;
        cur_file->lock_ptr = &files_lock[i];

        if (pthread_create(&thread_store[thread_ptr++], NULL, write_in_file, (void*)cur_file) != 0) {
            perror("Failed to create writer thread");
            free(cur_file);
            free(file_name);
            continue;
        }

            printf("Created writer thread %ld, PID %d, PPID %d, time %s\n",
               (long)thread_store[thread_ptr-1], getpid(), getppid(), get_cur_time_str());
        sleep(1);
    }

    // Create reader threads
    for (size_t i = 0; i < n/2; i++) {
        struct file_decr* cur_file = malloc(sizeof(struct file_decr));
        if (!cur_file) {
            perror("Failed to allocate file descriptor");
            continue;
        }

        cur_file->file_name = file_store[i];
        cur_file->lock_ptr = &files_lock[i];

        if (pthread_create(&thread_store[thread_ptr++], NULL, read_file, (void*)cur_file) != 0) {
            perror("Failed to create reader thread");
            free(cur_file);
            continue;
        }

        printf("Created reader thread %ld, PID %d, PPID %d, time %s\n", 
               (long)thread_store[thread_ptr-1], getpid(), getppid(), get_cur_time_str());
    }

    // Wait for all threads to complete
    void* status;
    for (size_t i = 0; i < n; i++) {
        pthread_join(thread_store[i], &status);
        int* res =(int*) &status;
        if (*res == 1)
              printf("Error occurred while opening file in %d\n", thread_store[i]);
		free(res);
        //printf("Status of thread %d is %d\n", thread_store[i], *res);
    }

    // Cleanup
    for (size_t i = 0; i < n/2; i++) {
        free(file_store[i]);
    }
	
	pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&m);
    return 0;
}
