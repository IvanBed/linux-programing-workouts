#include <stdio.h>
#include "store_ll.h"
#include <cstdlib>
#include <errno.h>

// Вынести в заголовчной файл.
/*
struct StoreNode {
    struct StoreNode *next;
    struct StoreNode *prev;
    int val;
};

struct Store {
    struct StoreNode *head;
	struct StoreNode *tail;
    size_t size;
    size_t capacity;    
};

*/
int store_init(struct Store **store, size_t capacity) {

    *store = (struct Store*)malloc(sizeof(struct Store));
    if (*store == NULL) {
        fprintf(stderr, "Could not allocate memory fo store instance.\n");
        return 1;
    }
    (*store)->capacity = capacity;
    (*store)->size = 0;
    (*store)->head = NULL;
	(*store)->tail = NULL;
    return 0;
}


int make_node(struct StoreNode** cur_node, int val) {

    *cur_node = (struct StoreNode*) malloc(sizeof(struct StoreNode));
    if (*cur_node == NULL) {
        fprintf(stderr, "Could not allocate memory for a new node.\n");
        return 1;
    }
    
    (*cur_node)->val = val;
    (*cur_node)->next = NULL;
    (*cur_node)->prev = NULL;
    return 0; 

}


int add_val(struct Store **store_inst, int val) {
	if (store_inst == NULL || *store_inst == NULL) {
        fprintf(stderr, "store_inst is NULL");
        return 1;
    }
    struct StoreNode *new_node = NULL;
    int res = make_node(&new_node, val);
    if (res != 0) {
        fprintf(stderr, "Could not make a new node.\n");
        return 1;
    }
    
	struct StoreNode *head = (*store_inst)->head;
    if ((*store_inst)->head == NULL) {
       (*store_inst)->head = new_node;
        (*store_inst)->tail = new_node; 
    } else {
        new_node->next = (*store_inst)->head;
        (*store_inst)->head->prev = new_node;
        (*store_inst)->head = new_node;
       
    }
    (*store_inst)->size++;
    return 0;

}


int get_val(struct Store **store_inst, int* ret_val) {
    if (store_inst == NULL || *store_inst == NULL) {
        fprintf(stderr, "Store_inst is NULL.\n");
        return 1;
    }
    struct StoreNode *last_node = (*store_inst)->tail;
	 
    if (last_node == NULL) {
        fprintf(stderr, "Last node is NULL.\n");
        return 1;
    }

	if (last_node->prev != NULL) {
        last_node->prev->next = NULL;
	    (*store_inst)->tail = last_node->prev;
	} else {
		(*store_inst)->head = NULL;
		(*store_inst)->tail = NULL;
	}  
	    
	*ret_val = last_node->val;
    
    free(last_node);
	(*store_inst)->size--;
    return 0;
}


int get_val2(struct Store **store_inst, int* ret_val) {
    if (store_inst == NULL || *store_inst == NULL) {
        fprintf(stderr, "Store is NULL.\n");
        return 1;
    }
    struct StoreNode *cur = (*store_inst)->head;
	 
    if (cur == NULL) {
        fprintf(stderr, "Current node is NULL.\n");
        return 1;
    }
 
    while (cur->next != NULL) cur = cur->next;
	struct StoreNode *last_node = cur;
	if (last_node->prev != NULL)
        last_node->prev->next = NULL;
	else 
	    (*store_inst)->head = NULL;
	
	*ret_val = last_node->val;
    
    free(last_node);
	last_node = NULL; 
	(*store_inst)->size--;
    return 0;
}


int free_store(struct Store *store_inst) {
    if (store_inst == NULL) {
        fprintf(stderr, "Store is empty\n"); 
		return 1;
    }
    struct StoreNode *cur = store_inst->head;
	struct StoreNode *temp = NULL;
    while (cur != NULL) {
        temp = cur;
        cur = cur->next;
        free(temp);
        temp = NULL;
		store_inst->size--;
    } 
    store_inst->head = NULL;
    return 0;
}


void print_list(struct Store *store_inst) {
	
	struct StoreNode *cur = store_inst->head;
    while (cur) {
        printf("val %d\n", cur->val);
        cur = cur->next;
    }
}

//int main() {return 0;}
