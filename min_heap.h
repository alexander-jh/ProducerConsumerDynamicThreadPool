#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define HEAP_SIZE       2500
#define MPANIC(x) ;     assert(x != NULL)
#define MIN(x, y)       (x < y) ? x : y

typedef struct heap_struct heap_t;

typedef struct data_struct data_t;

heap_t *create_heap(void);

void destroy_heap(heap_t *);

void swap(data_t *, data_t *);

int32_t get_right_child(heap_t *, int32_t);

int32_t get_left_child(heap_t *, int32_t);

int32_t get_parent(heap_t *, int32_t);

void min_heapify(heap_t *, int32_t);

int32_t minimum(heap_t *);

int32_t get_priority(heap_t *, uint32_t);

void *extract_min(heap_t *);

void decrease_key(heap_t *, void *, int32_t, int32_t);

void insert(heap_t *, void *, int32_t);