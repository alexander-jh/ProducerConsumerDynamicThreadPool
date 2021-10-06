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
    for(i = 0; i < HEAP_SIZE; ++i)
        h->data[i] = malloc(sizeof(data_t));
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
    data_t *t = a;
    *a = *b;
    *b = *t;
}

int32_t get_right_child(heap_t *h, int32_t i) {
    return ((2 * i) + 1 < h->capacity && i > 0) ? 2 * i + 1 : -1;
}

int32_t get_left_child(heap_t *h, int32_t i) {
    return ((2 * i) < h->capacity && i > 0) ? 2 * i : -1;
}

int32_t get_parent(heap_t *h, int32_t i) {
    return (i > 1 && i < h->capacity) ? i / 2 : -1;
}

int32_t get_priority(heap_t *h, uint32_t i) {
    return h->data[i]->priority;
}

void min_heapify(heap_t *h, int32_t i) {
    int32_t l, r, min;

    l   = get_left_child(h, i);
    r   = get_right_child(h, i);
    min = i;

    if(l <= h->size && l > 0) min = MIN(get_priority(h, min), get_priority(h, l));
    if(r <= h->size && r > 0) min = MIN(get_priority(h, min), get_priority(h, r));
    if(min != i) {
        swap(h->data[i], h->data[min]);
        min_heapify(h, min);
    }
}

int32_t minimum(heap_t *h) {
    return (h->size > 0) ? get_priority(h, 1) : -1;
}

void *extract_min(heap_t *h) {
    void *data = NULL;
    if(h->size > 0) {
        data = h->data[1]->data;
        h->size--;
        min_heapify(h, 1);
    }
    return data;
}

void decrease_key(heap_t *h, void *d, int32_t k, int32_t i) {
    h->data[i]->data = d;
    h->data[i]->priority = k;
    while(i > 1 && get_priority(h, get_parent(h, i)) > get_priority(h, i)) {
        swap(h->data[i], h->data[get_parent(h, i)]);
        i = get_parent(h, i);
    }
}

void insert(heap_t *h, void *d, int32_t i) {
    ++h->size;
    decrease_key(h, d, i, h->size);
}