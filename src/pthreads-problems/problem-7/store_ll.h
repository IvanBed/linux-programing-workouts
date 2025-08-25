#ifndef STORE_LL_H
#define STORE_LL_H

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

int store_init(struct Store**, size_t);

int add_val(struct StoreNode**, int);

int get_val(struct StoreNode**, int*);

int free_store(struct StoreNode*); 


#endif
