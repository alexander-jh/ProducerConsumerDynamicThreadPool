/*
 * MPMC queue built using a ring buffer and semaphore signaling
 * between enqueue and dequeue operations. Inner push and pop
 * functions carry out the logic and the outside function
 * ensures mutual exclusion. This is a bounded buffer.
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

// Void pointer array is used for data so the queue can
// be repurposed throughout this project.
typedef struct atomic_queue_struct atomic_queue_t;

// Default constructor.
//      capacity    -   integer value of maximum buffer capacity
//      data_p      -   byte size of object to be held in data
atomic_queue_t *atomic_queue_create(int capacity, size_t data_p);

// Default constructor.
void atomic_queue_destroy(atomic_queue_t *q);

// Inner, non-thread safe function for executing insertion logic
// on the queue.
//      data        -   reference to item to be inserted
//      is_wq       -   identifies if insertion is into work queue to
//                      update queue_position for transform_t
void _atomic_queue_push(atomic_queue_t *q, void *data, bool is_wq);

// Thread safe wrapper for insertion that guarantees mutual exclusion.
void atomic_queue_push(atomic_queue_t *q, void *data, bool is_wq);

// Inner, non-thread safe function for executing dequeue logic.
void *_atomic_queue_pop(atomic_queue_t *q);

// Thread safe wrapper for dequeue.
void *atomic_queue_pop(atomic_queue_t *q);

// Thread safe return of the current queue size.
int atomic_queue_size(atomic_queue_t *q);
