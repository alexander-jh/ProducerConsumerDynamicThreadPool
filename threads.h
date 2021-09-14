#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>

#include "atomic_queue.h"
#include "transform.h"
#include "transformMat.h"

#define PRODUCER_THREADS        4
#define CONSUMER_THREAD_MAX     20
#define INPUT_BUFFER_MAX        85
#define WORK_BUFFER_SIZE        200
#define WORK_MAX_THRESH         150
#define WORK_MIN_THRESH         50
#define OUTPUT_BUFFER_MAX       200

#define THREAD_STOPPING         0x1
#define THREAD_RUNNING          0x2

#define SLEEP_INTERVAL          1

extern atomic_queue_t   *input_queue;
extern atomic_queue_t	*work_queue;
extern atomic_queue_t	*output_queue;

extern atomic_int       reader_done;
extern atomic_int		producer_done;
extern atomic_int		consumer_done;

extern atomic_int       writer_pos;

typedef struct thread_struct thread_t;

thread_t *create_thread(atomic_queue_t *, void* (*)(void *));

void thread_delete(thread_t *);

/*
 * Creates the three major blocking queue types.
 */
void make_queues(void);

/*
 * Default destructor for queue objects.
 */
void destroy_queues(void);

/*
 * Declares signaling extern booleans and position of the current
 * writer operation.
 */
void set_booleans(void);

void *reader(void *arg);

void *producer(void *arg);

void *monitor(void *arg);

void *consumer(void *arg);

void *writer(void *arg);

