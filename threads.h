#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>

#include "atomic_queue.h"
#include "transform.h"
#include "transformMat.h"

#define PRODUCER_THREADS        4
#define CONSUMER_THREAD_MAX     20
#define INPUT_BUFFER_MAX        200
#define WORK_BUFFER_SIZE        200
#define WORK_MAX_THRESH         150
#define WORK_MIN_THRESH         50
#define OUTPUT_BUFFER_MAX       200

#define BELOW_LOWER_BOUND       0x1
#define ABOVE_LOWER_BOUND       0x2
#define BELOW_HIGHER_BOUND      0x3
#define ABOVE_HIGHER_BOUND      0x4
#define WORK_QUEUE_FULL         0x5
#define WORK_QUEUE_EMPTY        0x6

#define THREAD_STOPPING         0x1
#define THREAD_RUNNING          0x2

#define SLEEP_INTERVAL          1

typedef struct thread_struct thread_t;

typedef struct flag_struct flag_t;

extern flag_t           *flags;

extern atomic_queue_t   *input_queue;
extern atomic_queue_t	*work_queue;
extern atomic_queue_t	*output_queue;
extern atomic_queue_t	*run_queue;

extern atomic_int       reader_done;
extern atomic_int		producer_done;
extern atomic_int		consumer_done;

extern atomic_int       writer_pos;

flag_t *create_flags(void);

void flag_delete(flag_t *f);

thread_t *create_thread(void* (*)(void *));

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

void report_status(int size, char cmd, int pos, uint16_t key);

void update_flag_status(int size);