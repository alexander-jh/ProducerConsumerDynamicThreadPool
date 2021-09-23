/**
 * Bounded MPMC queue built using a ring buffer and 2 mutex
 * locks and semaphores. Inner push and pop functions carry
 * out the logic and the outside function ensures mutual
 * exclusion. This is a bounded buffer.
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
#include <unistd.h>

/**
 * Void pointer array is used for data so the queue can
 * be repurposed throughout this program.
 */
typedef struct atomic_queue_struct atomic_queue_t;

/**
 * Default constructor.
 * @param  - integer value of maximum buffer capacity
 * @param  - byte size of object to be held in data
 * @return - created queue struct
 */
atomic_queue_t *atomic_queue_create(int, size_t);

/**
 * Default constructor.
 * @param - reference to queue
 */
void atomic_queue_destroy(atomic_queue_t *q);

/**
 * Thread safe function to add an element to the queue. Will
 * continue execution until successful.
 * @param  - reference to a queue
 * @param  - element to be added
 * @return - queue position of added element
 */
int _atomic_queue_push(atomic_queue_t *, void *);
int atomic_queue_push(atomic_queue_t *, void *);

/**
 * Thread safe function to remove the eldest element from
 * the queue. Will run until successful.
 * @param  - reference to a queue
 * @return - reference to removed element
 */
void *_atomic_queue_pop(atomic_queue_t *q);
void *atomic_queue_pop(atomic_queue_t *q);

/**
 * Thread safe implementation to remove the eldest element
 * from the FIFO queue. Not guaranteed to be successful.
 * Will return NULL in the case of an empty queue.
 * @param  - reference to a queue
 * @return - reference to removed element
 */
void *try_queue_pop(atomic_queue_t *q);

/**
 * Returns size of queue. Guaranteed atomic but is non-locking
 * and does not guarantee exclusion.
 * @param  - reference to a queue
 * @return - returns the size of the queue
 */
int atomic_queue_size(atomic_queue_t *q);
