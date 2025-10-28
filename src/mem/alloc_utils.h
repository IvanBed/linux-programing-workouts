#ifndef ALLOC_UTILS
#define ALLOC_UTILS

#define FREE 1
#define ALLOCATED 0
#define METAINFO_SIZE 40

#define HEADER_SIZE 4 // count of cells that store the chunk's metainfo
#define SIZE_OFFSET 4
#define NEXT_OFFSET 3
#define PREV_OFFSET 2
#define L_BORDER_MARKER_OFFSET 1

#include <stdlib.h>
#include <unistd.h>

size_t* chunk_init_metainfo(size_t *);

void set_size(size_t *, size_t);

size_t get_size(size_t *);

void set_next_chunk_ptr(size_t *, size_t *);

size_t * get_next_chunk_ptr(size_t *);

void set_prev_chunk_ptr(size_t *, size_t *);

size_t * get_prev_chunk_ptr(size_t *);

void set_left_border_marker(size_t *, size_t);

size_t get_left_border_marker(size_t *);

void set_right_border_marker(size_t *, size_t, size_t);

size_t get_right_border_marker(size_t *);

size_t * make_chunk(size_t *, size_t, size_t); 

size_t * find_chunk(size_t *, size_t);

void add_next(size_t *, size_t *);

void merge_chunks(size_t *, size_t *);


void free_chunk(size_t *);

void  print_list(size_t *);


#endif
