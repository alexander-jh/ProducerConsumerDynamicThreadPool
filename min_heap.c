#include <stdio.h>
#include "min_heap.h"

struct heap_struct {
    int32_t         capacity;
    int32_t         size;
    data_t          **data;
};

struct data_struct {
    uint16_t        priority;
    void            *data;
};

heap_t *create_heap() {
    uint32_t i;
    heap_t *h   = (heap_t *) malloc(sizeof(heap_t)) MPANIC(h);
    h->data     = (data_t **) malloc(HEAP_SIZE * sizeof(data_t *)) MPANIC(h->data);
    for(i = 0; i < HEAP_SIZE; ++i) {
        h->data[i] = malloc(sizeof(data_t));
        h->data[i]->priority = UINT16_MAX;
    }
    h->size     = 0;
    h->capacity = HEAP_SIZE;
    return h;
}

void destroy_heap(heap_t *h) {
    int32_t i;
    for(i = 0; i < h->capacity; ++i)
        if(h->data[i]) free(h->data[i]);
    free(h);
}

void swap(data_t *a, data_t *b) {
    data_t t = *a;
    *a = *b;
    *b = t;
}

uint16_t get_priority(heap_t *h, int32_t i) {
    return h->data[i]->priority;
}

void min_heapify(heap_t *h, int32_t i) {
    int32_t l, r, min;

    l   = LEFT(i);
    r   = RIGHT(i);
    min = i;

    if(l < h->size && get_priority(h, l) < get_priority(h, min)) min = l;
    if(r < h->size && get_priority(h, r) < get_priority(h, min)) min = r;
    if(get_priority(h, i) != get_priority(h, min)) {
        swap(h->data[i], h->data[min]);
        min_heapify(h, min);
    }
}

uint16_t minimum(heap_t *h) {
    return (h->size > 0) ? get_priority(h, 0) : 0;
}

void *extract_min(heap_t *h) {
    void *data = NULL;
    if(h->size) {
        h->size--;
        swap(h->data[0], h->data[h->size]);
        min_heapify(h, 0);
        data = h->data[h->size]->data;
    }
    return data;
}

void decrease_key(heap_t *h, int32_t i) {
    uint16_t priority = get_priority(h, i);
    while(i > 0 && get_priority(h, PARENT(i)) > priority) {
        swap(h->data[i], h->data[PARENT(i)]);
        i = PARENT(i);
    }
}

void insert(heap_t *h, void *d, uint16_t i) {
    h->data[h->size]->data = d;
    h->data[h->size]->priority = i;
    decrease_key(h, h->size);
    ++h->size;
}