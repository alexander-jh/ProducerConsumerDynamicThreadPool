/*
 * Blocking queue to handle mutual exclusion for buffer. Is abstracted
 * to handle all three major queue types.
 */

#ifndef PROJECT1_ATOMIC_QUEUE_H

#include <assert.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdbool.h>

#include "transform.h"

#define PROJECT1_ATOMIC_QUEUE_H

typedef struct atomic_queue_struct atomic_queue_t;

atomic_queue_t *atomic_queue_create(uint16_t size, size_t task);

void atomic_queue_destroy(atomic_queue_t *q);

void atomic_queue_add(atomic_queue_t *q, void *ele);

void *atomic_queue_remove(atomic_queue_t *q, bool is_wq);

void *_atomic_queue_remove(atomic_queue_t *q, bool is_wq);

void *_atomic_queue_try_remove(atomic_queue_t *q, bool is_wq);

#endif //PROJECT1_ATOMIC_QUEUE_H
