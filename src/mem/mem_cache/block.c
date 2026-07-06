#include "block.h"

void init_block_list(slab *slab, size_t object_size, size_t block_cnt)
{
    if (!slab)
    {
        printf("Can not init list of blocks: slab pointer is null\n");
        return;
    }

    block  block;
    size_t offset;
    struct block *prev_block;
    char  *raw_data_ptr;

    raw_data_ptr = (char*)slab->raw_data;

    offset = object_size + sizeof(struct block);

    block.size = object_size;
    block.next_block = NULL;
    block.slab_ptr = (void*)slab;

    memcpy(raw_data_ptr, (char*)&block, sizeof(struct block));

    slab->first_block = (struct block*)raw_data_ptr;

    prev_block = slab->first_block;

    size_t index = 1;
    for ( ;index < block_cnt; index++)
    {
        raw_data_ptr += offset;
        block.size = object_size;
        block.next_block = NULL;
        memcpy(raw_data_ptr, (char*)&block, sizeof(block));

        prev_block->next_block = (struct block*)raw_data_ptr;
        prev_block = (struct block*)raw_data_ptr;
    }
}

static size_t calc_block_size(size_t object_size)
{
    size_t block_size = sizeof(block) + object_size;
    return block_size;

}

void *alloc_block(slab *slab)
{
    if (slab->size >= slab->capacity)
    {
        printf("Can not allocate a new block: slab is full\n");
        return NULL;
    }

    struct block *block      = slab->first_block;
    slab->first_block = slab->first_block->next_block;
    slab->size++;
    block->next_block = NULL;

    return (void*)((char*)block + sizeof(struct block));
}

void free_block(slab *slab, block *target_block)
{
    if (slab->size == 0)
    {
        printf("Slab is empty\n");
        return;
    }
        
    if (slab->first_block == target_block)
    {
        printf("Target block is already free\n");
        return;
    }    

    target_block->next_block = slab->first_block;
    slab->first_block = target_block;
    slab->size--;
}

void test_blocks_list(slab *slab)
{
    block *block = slab->first_block;
    size_t index = 0;
    while(index < slab->size)
    {
        block = block->next_block;
        index++;
    }
}