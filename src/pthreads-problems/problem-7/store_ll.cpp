#include <stdio.h>
#include "store_ll.h"
#include <cstdlib>

// Вынести в заголовчной файл.
/*
struct StoreNode {
    
    struct StoreNode *next;
    struct StoreNode *prev;
    int val;

}

struct Store {

    struct StoreNode *head;
    size_t size;
    size_t capacity;    
}

*/


int store_init(struct Store **store, size_t capacity) {

    *store = (struct Store*)malloc(sizeof(struct Store));
    if (*store == NULL) {
        puts( "Could not allocate memory fo store instance\n");
        return 1;
    }
    (*store)->capacity = capacity;
    (*store)->size = 0;
    return 0;
}


int make_node(struct StoreNode** cur_node, int val) {

    *cur_node = (struct StoreNode*) malloc(sizeof(struct StoreNode));
    if (*cur_node == NULL) {
        puts("Could not allocate memory for a new node\n");
        return 1;
    }
    
    (*cur_node)->val = val;
    (*cur_node)->next = NULL;
    (*cur_node)->prev = NULL;
    return 0; 

}


int add_val(struct StoreNode** store_inst, int val) {

    struct StoreNode *new_node = NULL;
    int res = make_node(&new_node, val);
    if (res != 0) {
        puts("Could not make a new node\n");
        return 1;
    }

    if (store_inst == NULL) {
       *store_inst = new_node;

    } else {
        new_node->prev = *store_inst;
        (*store_inst)->next = new_node;
        (*store_inst) = new_node;

    }

    return 0;

}


int get_val(struct StoreNode** store_inst, int* ret_val) {

    struct StoreNode *cur = *store_inst;
    while (cur->next != NULL) cur = cur->next;
    
    *ret_val = cur->val;
    struct StoreNode *temp = cur;
    cur->prev->next = NULL;
    free(temp);
    return 0;

}


int free_store(struct StoreNode* store_inst) {
    if (store_inst == NULL) {
        puts("Store is empty\n"); 
    }
    struct StoreNode *cur = store_inst;
    while (cur != NULL) {
        struct StoreNode *temp = cur;
        cur = cur->next;
        free(temp);
    } 

    return 0;
}


void print_list(struct StoreNode *head) {
    while (head) {
        printf("val %d\n", head->val);
        head = head->next;
    }


}

int main() {return 0;}
