#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "atomic_queue.h"
#include "transform.h"
#include "transformMat.h"

#define PRODUCER_THREADS        4
#define CONSUMER_THREAD_MAX     20
#define INPUT_BUFFER_COUNT      3
#define INPUT_BUFFER_MIN        50
#define INPUT_BUFFER_MAX        200
#define WORK_BUFFER_SIZE        200
#define WORK_MAX_THRESH         150
#define WORK_MIN_THRESH         50
#define OUTPUT_BUFFER_COUNT     3
#define OUTPUT_BUFFER_MIN       50
#define OUTPUT_BUFFER_MAX       200

extern atomic_queue_t   *input_queue;
extern atomic_queue_t	*work_queue;
extern atomic_queue_t	*output_queue;

extern bool             reader_done;
extern bool				producer_done;
extern bool				consumer_done;

extern uint16_t         writer_pos;

/*
 * Creates the three major blocking queue types.
 */
void make_queues(void);

/*
 * Declares signaling extern booleans and position of the current
 * writer operation.
 */
void set_booleans(void);

void *reader(void *arg);

void *producer(void *arg);

void *consumer(void *arg);

void *writer(void *arg);
