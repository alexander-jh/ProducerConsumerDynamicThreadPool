#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define HEAP_SIZE       2500
#define MPANIC(x) ;     assert(x != NULL)
#define LEFT( i )       ((2 * i) + 1)
#define RIGHT( i )      ((2 * i) + 2)
#define PARENT( i )     ((i - 1) / 2)

typedef struct heap_struct heap_t;

typedef struct data_struct data_t;

heap_t *create_heap(void);

void destroy_heap(heap_t *);

void swap(data_t *, data_t *);

void min_heapify(heap_t *, int32_t);

uint16_t minimum(heap_t *);

uint16_t get_priority(heap_t *, int32_t);

void *extract_min(heap_t *);

void decrease_key(heap_t *, int32_t);

void insert(heap_t *, void *, uint16_t);

int heap_size(heap_t *);