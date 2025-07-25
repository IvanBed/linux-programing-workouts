#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>

#define true 1
#define false 0


static volatile int stop_sort_thread = false;

struct Node {
    char* line;
    struct Node* next;
    struct Node* prev;
	pthread_mutex_t m;
};

static int cpy_str(char** new_str, char* old_str) {
    if (old_str == NULL) {
        *new_str = NULL;
        return 0;
    }
    
    size_t str_len = strlen(old_str) + 1;
    *new_str = (char*)malloc(str_len);
    
    if (*new_str == NULL) {
        perror("Memory for the string can not be allocated.");
        return 1;
    }
    
    memcpy(*new_str, old_str, str_len);
    return 0;
} 

static int new_node(struct Node** node, char* val) {
    *node = (struct Node*)malloc(sizeof(struct Node));
    
    if (*node == NULL) {
        perror("Memory for the node can not be allocated.");
        return 1;
    }
    
    char* str;
    cpy_str(&str, val);
    (*node)->line = str;
    (*node)->next = NULL;
    (*node)->prev = NULL;
	pthread_mutex_init(&(*node)->m, NULL);
    return 0;
}

static int add_to_head(struct Node** list, char* val) {
    struct Node* node; 
    int res = new_node(&node, val);

    if (res != 0) {
        return 1;
    }
    
    if (*list == NULL) {
        *list = node;
    } else {
        
        pthread_mutex_lock(&(*list)->m);

        node->next = *list;
        (*list)->prev = node;
        *list = node;   

        pthread_mutex_unlock(&(*list)->next->m);
		
    }
    
    return 0;
}

static int add_to_tail(struct Node** list, char* val) {
    struct Node* node; 
    int res = new_node(&node, val);
    
    if (res != 0) {
        return 1;
    }
    
    if (*list == NULL) {
        *list = node;
    } else {
        struct Node* cur = *list;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = node;
        node->prev = cur;
    }
    
    return 0;
}

static int list_free(struct Node* list) {
    if (list == NULL) {
        perror("List is empty");
        return 1;
    }
    
    struct Node* temp = NULL;
    while (list) {
        temp = list;
        list = list->next;
		free(temp->line);
		pthread_mutex_destroy(&(temp->m));
        free(temp);
    }
    
    return 0;
}

static int print_list(struct Node* list) {
	if (list == NULL) {
		fprintf(stderr, "List is empty\n");
		return 1;
	}
    
    struct Node* cur = list;
    pthread_mutex_lock(&(cur->m));    
    
    while (cur) {
        printf("current node value: %s\n", cur->line);
        
        struct Node* next = cur->next;
        if (next) {
            pthread_mutex_lock(&(next->m));  
        }
        
        pthread_mutex_unlock(&(cur->m));   
        cur = next;   
    }
    
    return 0;
}

static int read_lines(struct Node** list) {
    char cur_line[81];
    int res = 0;
    
    while (true) {
        //fgets(cur_line, sizeof(cur_line), stdin);
        if (fgets(cur_line, sizeof(cur_line), stdin) == NULL) {
			break;
		}
		
		cur_line[strcspn(cur_line, "\n")] = '\0';
        
        if (strcmp(cur_line, "c_exit") == 0) {
            break;
        }
        
       
              
        if (strcmp(cur_line, "") == 0) {
            res = print_list(*list);
        } else {
            res = add_to_head(list, cur_line); 
        }


       
    }    
    stop_sort_thread = true;
    return res;
}

static int swap(struct Node** l_node, struct Node** r_node, struct Node** list_head) {
    if (l_node == NULL || r_node == NULL || *l_node == NULL || *r_node == NULL) {
        return 1;
    }

    struct Node* a = (*l_node)->prev;
    struct Node* b = *l_node;
    struct Node* c = *r_node;
    struct Node* d = (*r_node)->next;

    // Проверяем, что узлы действительно соседние
    if (b->next != c) {
        return 1;
    }

    // Блокируем в строгом порядке от начала к концу списка
    if (a) pthread_mutex_lock(&a->m);
    pthread_mutex_lock(&b->m);
    pthread_mutex_lock(&c->m);
    if (d) pthread_mutex_lock(&d->m);

    // Выполняем перестановку
    b->next = d;
    if (d) d->prev = b;

    if (a) a->next = c;
    c->prev = a;

    c->next = b;
    b->prev = c;

    // Обновляем голову списка если нужно
    if (b == *list_head) {
        *list_head = c;
    }

    // Разблокируем в обратном порядке
    if (d) pthread_mutex_unlock(&d->m);
    pthread_mutex_unlock(&c->m);
    pthread_mutex_unlock(&b->m);
    if (a) pthread_mutex_unlock(&a->m);

    return 0;
}

// переписать сортировку, с ней основные проблемы

static void* bubble_sort_mt(void* data) {
    while (!stop_sort_thread) {
        struct Node** list_head = (struct Node**)data;
        
        // Ждём пока список будет готов к сортировке
        while (1) {
            // Блокируем голову для проверки
            if (*list_head) pthread_mutex_lock(&(*list_head)->m);
            if (list_head == NULL || *list_head == NULL || (*list_head)->next == NULL) {
                if (*list_head) pthread_mutex_unlock(&(*list_head)->m);
                sleep(1);
                continue;
            }
            break;
        }

        int is_swapped;
        struct Node* end = NULL;

        do {
            is_swapped = 0;
            struct Node* current = *list_head;
            struct Node* prev_locked = NULL;

            // Блокируем первый узел
            pthread_mutex_lock(&current->m);

            while (current->next != end) {
                struct Node* next_node = current->next;
                
                // Блокируем следующий узел
                pthread_mutex_lock(&next_node->m);

                if (strcmp(current->line, next_node->line) > 0) {
                    // Разблокируем предыдущий узел перед swap
                    if (prev_locked) pthread_mutex_unlock(&prev_locked->m);
                    
                    swap(&current, &next_node, list_head);
                    is_swapped = 1;
                    
                    // После swap current и next_node поменялись местами
                    // current теперь указывает на узел, который был next_node
                    pthread_mutex_unlock(&current->m);
                    current = next_node;
                    pthread_mutex_lock(&current->m);
                } else {
                    // Переходим к следующему узлу
                    if (prev_locked) pthread_mutex_unlock(&prev_locked->m);
                    prev_locked = current;
                    current = next_node;
                }
            }

            // Разблокируем последний узел в проходе
            pthread_mutex_unlock(&current->m);
            if (prev_locked) pthread_mutex_unlock(&prev_locked->m);

            end = current;
        } while (is_swapped && !stop_sort_thread);

        sleep(5);
    }
    puts("Stop sorting thread.");
    return NULL;
}

int main(int argc, char** argv) {
    struct Node* list = NULL;
    pthread_t sort_tread;
    
	
	if (pthread_create(&sort_tread, NULL, bubble_sort_mt, (void*)&list)) {
        perror("pthread_create() error.");
        return 1;
    }
	
    puts("Lets start...");
    int res = read_lines(&list);
	
	pthread_join(sort_tread, NULL);
    
	
    list_free(list);
    return res;
}
