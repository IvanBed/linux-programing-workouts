#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <cassert>
using namespace std;

struct Node{
    Node *next;
    Node *prev;
    int value;
    int key;
    Node(Node *p, Node *n, int k, int val):prev(p),next(n),key(k),value(val){};
    Node(int k, int val):prev(NULL),next(NULL),key(k),value(val){};
};

class Cache{
    protected: 
    map<int,Node*> mp; //map the key to the node in the linked list
    int cp;  //capacity
    Node *tail; // double linked list tail pointer
    Node *head; // double linked list head pointer
    virtual void set(int, int) = 0; //set function
    virtual int get(int) = 0; //get function
};

class LRUCache : public Cache {
    
	private: 
        size_t act_size;
        
        void moveToHead(Node* node) {
            if(node->next != nullptr) {
                node->next->prev = node->prev;
            }
            if(node->prev != nullptr) {
                node->prev->next = node->next;
            }
            head->next = node;
            node->next = NULL;
            node->prev = head;
            head = node;
        }
        
    public:
        LRUCache(int cp) {
           this->cp = cp;
           this->mp = map<int, Node*>();
           this->head = nullptr;
           this->tail = nullptr;
           this->act_size = 0;
        }
        
        void set(int key, int value) {
            Node *new_node = new Node(key, value);
            if(mp.count(key) == 0) {
                if(tail == NULL) {
                    head = new_node;
                    tail = new_node;
                    act_size++;
                } else {
                    head->next = new_node;
                    new_node->prev = head;
                    head = new_node; 
                    if(act_size < cp) {
                        act_size++;
                    } else {
                        Node* temp = tail;
                        if(tail->next != nullptr)
                            tail = tail->next;
                        delete temp;
                        mp.erase(temp->key); 
                    } 
                }
                
                mp[key] = new_node;
                
            } else {
               moveToHead(mp[key]);
               mp[key]->value = value;
            }
            
        } 
        int get(int key) {
            if (this->mp.count(key) == 0) {
                return -1;
            } else {
                moveToHead(mp[key]);
                return this->mp[key]->value; 
            }
        }   
};