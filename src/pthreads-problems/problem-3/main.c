#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

#define true 1
#define false 0

struct Node {
	char* line;
	struct Node* next;
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
	}
	return 0;
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
     if (list == NULL) {
         puts("List is empty");
         return 4;
     }
    struct Node* cur = list;
   
    while (cur) {
        printf("current node value: %s\n", cur->line);
        cur = cur->next;
    }
    return 0;
}

static int read_lines(struct Node** list) {
	if (list == NULL) {  
        puts("List pointer is null. Please initialize the list!");
        return 3;
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



// Пока набросок

static int bubble_sort(struct Node** list) {
	
	struct Node* cur_i = *list;
	struct Node* prev_j = cur_i;
	struct Node* cur_j = cur_i->next;
	int is_swaped = false;
	while(cur_i) {
		
		while(cur_j) {
			if (prev_j->line > cur_j->line) {
				//swap(prev_j, cur_j);
			}
			cur_j = cur_j->next;
		}
		if (!is_swaped) 
			break;
		cur_i = cur_i->next;
	}
	
	
	return 1;
}


int main(int argc, char** argv) {
	
	struct Node* list = NULL;
	//pthread_t read_tread;
	//pthread_create(read_tread, NULL, read_lines_mt, (void*)&list);
    
	int res = read_lines(&list);
	
	return res;
}
