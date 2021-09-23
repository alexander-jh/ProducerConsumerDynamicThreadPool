#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>

#include "atomic_queue.h"

#define PRODUCER_THREADS        4
#define CONSUMER_THREAD_MAX     10
#define INPUT_BUFFER_MAX        200
#define WORK_BUFFER_SIZE        200
#define WORK_MAX_THRESH         150
#define WORK_MIN_THRESH         50
#define OUTPUT_BUFFER_MAX       200
#define BURN_THRESHOLD          20

#define EMPTY_BUFFER            0x8
#define BELOW_LOWER             0x0
#define ABOVE_LOWER             0x4
#define ABOVE_UPPER             0x6
#define FULL_BUFFER             0x7

#define THREAD_STOPPING         0x1
#define THREAD_RUNNING          0x2

#define SLEEP_INTERVAL          1

/**
 * Struct for containing the consumer threads. This is necessary
 * to have a shared memory space so the monitor thread can softly
 * shut down threads and ensure the operation completes.
 */
typedef struct thread_struct thread_t;

/**
 * Struct to contain all the required values throughout the process
 * (char) cmd, (uint16_t) key, (int) sequence_number,
 * (int) work_queue_position, (uint16_t) encoded and decoded key,
 * (double) encoded and decoded return from transforms.
 */
typedef struct transform_struct transform_t;

/**
 * Globally declared queues.
 */
extern atomic_queue_t           *input_queue,
                                *output_queue,
                                *work_queue,
                                *run_queue;
extern atomic_int               produced,
                                written,
                                reader_done;
extern _Atomic(uint16_t)        state;
extern _Atomic(void *)          *current_task;
extern time_t                   start,
                                producer_start,
                                consumer_start,
                                end,
                                producer_end,
                                consumer_end;

/**
 * No-arg constructor for transform object.
 */
transform_t *transform_create(void);

/**
 * Destructor for the transform struct.
 * @param   - reference to a transform struct to free
 */
void task_destroy(transform_t *);

/**
 * Default constructor for the thread object. Requires a reference
 * to the function to execute for p_thread instantiation.
 * @param   - reference to thread safe function
 * @return  - pthread instantiated to referenced function
 */
thread_t *create_thread(void* (*)(void *));

/**
 * Default destructor for the thread struct.
 * @param   - reference to thread struct
 */
void thread_delete(thread_t *);

/**
 * Implemented in executor.c. Acts as a constructor for all queues
 * used in this implementation.
 */
void make_queues(void);

/**
 * Implemented in executor.c. Generalized destructor to remove
 * all created queue resources.
 */
void destroy_queues(void);

/**
 * Single thread reader responsible for parsing I/O, constructing
 * initial transform object and insertion to the input_queue.
 * @param   - not required for default action
 * @return  - returns trivial pthread args
 */
void *reader(void *);

/**
 * 4 threads for producers instantiated from executor.c. This runnable
 * is responsible for conducting the first series of transforms on
 * the data in the input queue. Following operation inserts transform
 * struct into work queue.
 * @param   - not required for default action
 * @return  - returns trivial pthread args
 */
void *producer(void *);

/**
 * Producer thread calls oversee the consumer's process. Consumer thread
 * count is dynamic so this thread regulates creation, sets flags for
 * when a soft shutdown is necessary (when items in the work queue are
 * below the lower threshold of work queue). Finished by shutting down
 * each thread. The major scenarios are:
 *    1) Producer not done and more items in queue than lower threshold
 *				=> Spawn a single thread
 *    2) Below thread capacity and upper bound is superseded
 *              => Spawn a single thread
 *    3) Producer not done and queue items are below lower threshold
 *              => Start soft landing threads quickly
 *    4) Producer finishes
 *              => Create threads to limit and finish tasks quickly
 * @param   - reference to transform struct
 */
void *monitor(void *);
void monitor_work(transform_t *);

/**
 * Consumes remaining work elements following the termination of producer
 * threads.
 */
void complete_consumption(void);

/**
 * Thread function for extracting items from the work queue, applying
 * second transform, reporting events to stderr, and insertion into
 * the output queue. In the event the thread must be termination, its
 * state value in the passed thread_t struct will be swapped from
 * IS_RUNNING to IS_STOPPED. On the start of the next execution this
 * loop will break and the thread stops.
 * @param   - not required for default action
 * @return  - returns trivial pthread args
 */
void *consumer(void *);

/**
 * Thread function. Outputs the data to stdin from the output queue.
 * @param   - not required for default action
 * @return  - returns trivial pthread args
 */
void *writer(void *arg);

/**
 * Returns if the state of reporting updated. If it did, swaps the current
 * state to the new one and returns true.
 * @param   - size of work queue
 * @return  - boolean signifying if the state changed
 */
bool get_state(int);

/**
 * If a change in state occurred, reports the relevant details to stderr.
 * @param   - reference to transform struct
 * @param   - new state of the process
 */
void report_state_change(void *, uint16_t);

/**
 * Reference to transform operations in attached object file transformMat.o.
 * @param   - key to be processed
 * @param   - reference to double for return value
 * @return  - processed key outcome
 */
uint16_t transformA1(uint16_t, double *);
uint16_t transformA2(uint16_t, double *);
uint16_t transformB1(uint16_t, double *);
uint16_t transformB2(uint16_t, double *);
uint16_t transformC1(uint16_t, double *);
uint16_t transformC2(uint16_t, double *);
uint16_t transformD1(uint16_t, double *);
uint16_t transformD2(uint16_t, double *);
uint16_t transformE1(uint16_t, double *);
uint16_t transformE2(uint16_t, double *);
