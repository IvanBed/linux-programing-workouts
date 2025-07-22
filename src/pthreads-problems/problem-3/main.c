#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>


#define true 1
#define false 0

static pthread_mutex_t m;
static pthread_cond_t cond;
static int cond_predicate = 0;


struct Node {
	char* line;
	struct Node* next;
	struct Node* prev;
};

static int cpy_str(char** new_str, char* old_str) {
    if (old_str == NULL) {  
        *new_str = NULL;
        return 0;
    }
    size_t str_len = strlen(old_str) + 1;
    *new_str = (char*) malloc(str_len);
    if (*new_str == NULL) {
		puts("Memory for the string can not be allocated.");
		return 1;
	}
    memcpy(*new_str, old_str, str_len);
    return 0;
} 

static int new_node(struct Node** node, char* val) {
    
	*node = (struct Node*) malloc(sizeof(struct Node));
	
	if (*node == NULL) {
		puts("Memory for the node can not be allocated.");
		return 1;
	}
	
    char* str;
    cpy_str(&str, val);
	(*node)->line = str;
	(*node)->next = NULL;
	(*node)->prev = NULL;
	return 0;
}


static int add_to_head(struct Node** list, char* val) {
	
	struct Node* node; 
	int res = new_node(&node, val);

	if (res != 0) {
		return 1;
	}
	if (*list == NULL)
		*list = node;
	else {
		node->next = (*list)->next;
		node->prev = *list;
		(*list)->next = node;
	}
	return 0;
}


static int add_to_tail(struct Node** list, char* val) {
	struct Node* node; 
	int res = new_node(&node, val);
	if (res != 0) {
		return 1;
	}
	if (*list == NULL)
		*list = node;
	else {
		struct Node* cur = *list;
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = node;
		node->prev = cur;
	}
	return 0;
}


static int create_list(struct Node** list) {
    
    int res = add_to_head(list, "start_list_ptr");
    
    return res;
}


static int list_free(struct Node* list) {
    
	if (list == NULL) {
		puts("List is empty");
		return 4;
	}
	struct Node* temp = NULL;
    while (list) {
		temp = list;
		list = list->next;
		free(temp);
	}
    
    return 0;
}

static int print_list(struct Node* list) {
     if (list == NULL || list->next == NULL) {
         puts("List is empty");
         return 4;
     }
    struct Node* cur = list->next;
   
    while (cur) {
        printf("current node value: %s\n", cur->line);
        cur = cur->next;
    }
    return 0;
}

static int read_lines(struct Node** list) {
	
	char cur_line[81];
	while (true) {
	    //pthread_mutex_lock(&m);
	    //while(cond_predicate == 1)
		//    pthread_cond_wait(&cond, &m);
	    //cond_predicate = 1;
	    fgets(cur_line, sizeof(cur_line), stdin);
		cur_line[strcspn(cur_line, "\n")] = '\0';
		if (strcmp(cur_line, "c_exit") == 0) {
			break;
		}
		if (strcmp(cur_line, "c_print") == 0) {
			print_list(*list);
		} else 
		    add_to_tail(list, cur_line);
		//cond_predicate = 0;
		//printf("cond pred %d\n", cond_predicate);
		//pthread_cond_signal(&cond);
	   // pthread_mutex_unlock(&m);
	}	
	

}

// Пока набросок

static void* read_lines_mt(void* data) {
	struct Node** list = (struct Node**) data;
	if (list == NULL) {  
        puts("List pointer is null. Please initialize the list!");
        return NULL;
    }

	char cur_line[81];
	while (true) {
	    fgets(cur_line, sizeof(cur_line), stdin);
		cur_line[strcspn(cur_line, "\n")] = '\0';
		if (strcmp(cur_line, "c_exit") == 0) {
			break;
		}
		if (strcmp(cur_line, "c_print") == 0) {
			print_list(*list);
		} else 
		    add_to_tail(list, cur_line);
	}	
	
}


static int swap(struct Node** l_node, struct Node** r_node) {
	if (l_node == NULL || r_node == NULL)
		return 1;

	(*l_node)->next = (*r_node)->next;
	if ((*r_node)->next != NULL)
		(*r_node)->next->prev = *l_node;
	
	
	if ((*l_node)->prev != NULL)
	    (*l_node)->prev->next = *r_node;
	(*r_node)->prev = (*l_node)->prev;
	
	
	(*l_node)->prev = *r_node;
	(*r_node)->next = *l_node;
	return 0;
}


static void* bubble_sort_mt(void* data) {
	puts("bubble_sort_mt");
	while(1) {	
		pthread_mutex_lock(&m);
		struct Node** list = (struct Node**) data;
		while (list == NULL || *list == NULL || (*list)->next == NULL)
			sleep(1);
	 
		cond_predicate = 1;
	 
		int is_swapped;
		struct Node* end = NULL; 

		do {
			is_swapped = 0;
			struct Node* current = *list;

			while (current->next != end) {
				if (strcmp(current->line, current->next->line) > 0) {
					
					struct Node* next_node = current->next;

					 
					if (current == *list) {
						*list = next_node;
					}

				   
					swap(&current, &next_node);
					is_swapped = 1;

				  
					current = next_node;
				}
				current = current->next;
			}
			end = current; 
		} while (is_swapped);
		cond_predicate = 0;
		 
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&m);
		sleep(30); 
	}
	
		return NULL;
	
}


int main(int argc, char** argv) {
	
	struct Node* list = NULL;
	pthread_t sort_tread;
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&cond, NULL);
	
	pthread_create(&sort_tread, NULL, bubble_sort_mt, (void*)&list);
    
	int res = read_lines(&list);

	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&cond);
	return 0;
}
