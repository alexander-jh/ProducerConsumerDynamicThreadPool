/*
 * Blocking queue to handle mutual exclusion for buffer. Is abstracted
 * to handle all three major queue types.
 */
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "transform.h"

typedef struct queue_node_struct queue_node_t;

typedef struct atomic_queue_struct atomic_queue_t;

atomic_queue_t *atomic_queue_create(int size);

void atomic_queue_destroy(atomic_queue_t *q);

bool atomic_queue_push(atomic_queue_t *queue, void *data);

void *atomic_queue_pop(atomic_queue_t *queue, bool is_wq);

int atomic_queue_size(atomic_queue_t *queue);
