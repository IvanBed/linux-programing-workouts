#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SLAB_BLOCKS 5 
#define PAGE_SIZE 4096

void *alloc_slab(int order);

void free_slab(void *slab);

void *alloc_slab(int order)
{
    void *ptr = (void *) malloc(PAGE_SIZE * order);
    return ptr; 
}

void free_slab(void *slab)
{
    free(slab);
}

typedef struct block {
    size_t size;
    struct block *next_block;
} block;


typedef struct slab {
    size_t size;
    size_t blck_cnt;
    size_t blck_size;
    block *first_block;
    void  *raw_data;
    struct slab *prev_slab; 
    struct slab *next_slab; 
} slab;

typedef struct slab_list {
    size_t size;
    slab  *head;
} slab_list;

void init_block_list(slab *slab, size_t object_size, size_t blck_cnt)
{
    block  blck;
    size_t offset;
    block *prev_block;
    char  *raw_data_ptr;
    
    raw_data_ptr = (char*)slab->raw_data;

    offset = object_size + sizeof(block);
    
    blck.size = object_size;
    blck.next_block = NULL;
    
    memcpy(raw_data_ptr, (char*)&blck, sizeof(block));
    slab->first_block = (block*)raw_data_ptr;
    prev_block = slab->first_block;

    size_t index = 1;
    for ( ;index < blck_cnt; index++)
    {
        raw_data_ptr += offset;
        blck.size = object_size;
        blck.next_block = NULL;
        memcpy(raw_data_ptr, (char*)&blck, sizeof(block));
        
        prev_block->next_block = (block*)raw_data_ptr;
        prev_block = (block*)raw_data_ptr;
    }
    //printf("%d \n",  prev_block->next_block);
}

void *alloc_block(slab *slab)
{
    if (slab->size >= slab->blck_cnt)
        return NULL;
        
    void * mem        = (void*)slab->first_block;
    slab->first_block = slab->first_block->next_block;
    slab->size++;
    return mem;
}

void free_block(slab *slab, block *target_block)
{
    if (slab->size == 0)
        return;
    if (slab->first_block == target_block)
        return;
        
    target_block->next_block = slab->first_block;
    slab->first_block = target_block;
    slab->size--;
}

void test_blocks_list(slab *slab)
{
    block *block = slab->first_block;
    printf("%ld \n",  block);
    size_t index = 0;
    while(block)
    {
        block = block->next_block;
        printf("%ld \n",  block);
    }
}

void init_slab(slab *slab, size_t capacity, size_t object_size)
{
    slab->size        = 0;
    slab->blck_cnt    = capacity;
    slab->prev_slab   = NULL;
    slab->next_slab   = NULL;
    slab->raw_data    = alloc_slab(capacity);
    slab->blck_size   = object_size;
    init_block_list(slab, object_size, capacity);
}

void slab_list_init(slab_list *list)
{
    list->size = 0;
    list->head = NULL;
}

void test_slabs_list(slab_list *list)
{
    slab *slab = list->head;
    while(slab)
    {
        printf("%ld \n",  slab);
        slab = slab->prev_slab;
    }
}

void add_to_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        return;
    }

    if (!slab)
    {
        return;
    }
    
    if (list->size == 0 || !list->head)
    {
        list->head = slab;
        list->head->prev_slab = NULL;
        list->head->next_slab = NULL;
    }
    else 
    {
        slab->prev_slab = list->head;
        list->head->next_slab = slab;
        list->head = slab;
    }
    list->size++;
}

void delete_from_list(slab_list *list, slab *slab)
{
    if (!list)
    {
        return;
    }

    if (!slab)
    {
        return;
    }
    
    if (list->head == slab)
    {
        list->head = NULL;
    }
    else 
    {
        struct slab *next = slab->next_slab;
        struct slab *prev = slab->prev_slab;

        if (prev)
            prev->next_slab = slab->next_slab;

        if (next)
            next->prev_slab = slab->prev_slab;
        
    }

    slab->next_slab = NULL;
    slab->prev_slab = NULL;
    list->size--;
}

void slab_reposition(slab_list *src, slab_list *dest, slab *slab)
{
    delete_from_list(src, slab);
    add_to_list(dest, slab);
}

/**
 * Эта структура представляет аллокатор, вы можете менять
 * ее как вам удобно. Приведенные в ней поля и комментарии
 * просто дают общую идею того, что вам может понадобится
 * сохранить в этой структуре.
 **/ 
struct cache {
    /* список пустых SLAB-ов для поддержки cache_shrink */
    /* список частично занятых SLAB-ов */
    /* список заполненых SLAB-ов */
    slab_list *free_slabs;
    slab_list *partially_filled_slabs;
    slab_list *filled_slabs;
    
    size_t object_size; /* размер аллоцируемого объекта */
    int slab_order; /* используемый размер SLAB-а */
    size_t slab_objects; /* количество объектов в одном SLAB-е */ 
};

size_t calculate_PAGE_SIZE(size_t object_size)
{
	
}

int get_order(size_t object_size)
{
	if (object_size < PAGE_SIZE)
	{
		return 0;
	}
	else
	{
		return 0;
	}
}

/**
 * Функция инициализации будет вызвана перед тем, как
 * использовать это кеширующий аллокатор для аллокации.
 * Параметры:
 *  - cache - структура, которую вы должны инициализировать
 *  - object_size - размер объектов, которые должен
 *    аллоцировать этот кеширующий аллокатор 
 **/
void cache_setup(struct cache *cache, size_t object_size)
{
    slab_list_init(cache->free_slabs);
    slab_list_init(cache->partially_filled_slabs);
    slab_list_init(cache->filled_slabs);
 
    cache->object_size  = object_size;
    cache->slab_order   = object_size;
    cache->slab_objects = (SLAB_BLOCKS * PAGE_SIZE) / object_size;
}


/**
 * Функция освобождения будет вызвана когда работа с
 * аллокатором будет закончена. Она должна освободить
 * всю память занятую аллокатором. Проверяющая система
 * будет считать ошибкой, если не вся память будет
 * освбождена.
 **/
void cache_release(struct cache *cache)
{
    /* Реализуйте эту функцию. */
}


/**
 * Функция аллокации памяти из кеширующего аллокатора.
 * Должна возвращать указатель на участок памяти размера
 * как минимум object_size байт (см cache_setup).
 * Гарантируется, что cache указывает на корректный
 * инициализированный аллокатор.
 **/
void *cache_alloc(struct cache *cache)
{
    if (cache->free_slabs->size == 0 && cache->partially_filled_slabs->size == 0)
    {
        slab *slab;
        init_slab(slab, cache->slab_objects, cache->object_size);
        add_to_list(cache->partially_filled_slabs, slab);
        
    }
}


/**
 * Функция освобождения памяти назад в кеширующий аллокатор.
 * Гарантируется, что ptr - указатель ранее возвращенный из
 * cache_alloc.
 **/
void cache_free(struct cache *cache, void *ptr)
{
    /* Реализуйте эту функцию. */
}


/**
 * Функция должна освободить все SLAB, которые не содержат
 * занятых объектов. Если SLAB не использовался для аллокации
 * объектов (например, если вы выделяли с помощью alloc_slab
 * память для внутренних нужд вашего алгоритма), то освбождать
 * его не обязательно.
 **/
void cache_shrink(struct cache *cache)
{
    /* Реализуйте эту функцию. */
}

void test1()
{
    slab s;
    init_slab(&s, 5, 10);
    
    test_blocks_list(&s);
    void *mem = alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);
    alloc_block(&s);       
    puts("-------------------------------------------------");
    test_blocks_list(&s);
    puts("-------------------------------------------------");
    //printf("%ld\n", mem);
    free_block(&s, mem);
    free_block(&s, mem);
    free_block(&s, mem);
    puts("-------------------------------------------------");
    test_blocks_list(&s);
}

void test2()
{   
    slab s1;
    slab s2;
    slab s3;
    slab s4;

    slab_list list1;
    slab_list list2;

    init_slab(&s1, 5, 10);
    init_slab(&s2, 5, 10);
    init_slab(&s3, 5, 10);
    init_slab(&s4, 5, 10);

    slab_list_init(&list1);
    slab_list_init(&list2);

    add_to_list(&list1, &s1);
    add_to_list(&list1, &s2);
    add_to_list(&list1, &s3);
   
    add_to_list(&list2, &s4);

    test_slabs_list(&list1);
    puts("----------------");
    test_slabs_list(&list2);
    puts("----------------");
    puts("");
    puts("");
    
    
    printf("slab ptr: %ld\n", &s1);
    slab_reposition(&list1, &list2, &s1);
 
    test_slabs_list(&list1);
    puts("----------------");
    test_slabs_list(&list2);
    puts("----------------");   
}

int main() 
{

}
