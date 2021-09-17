#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
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

#define EMPTY_BUFFER            0x8
#define BELOW_LOWER             0x0
#define ABOVE_LOWER             0x4
#define ABOVE_UPPER             0x6
#define FULL_BUFFER             0x7

#define THREAD_STOPPING         0x1
#define THREAD_RUNNING          0x2

#define SLEEP_INTERVAL          5

// Struct for containing the consumer threads. This is necessary
// to have a shared memory space so the monitor thread can softly
// shut down threads and ensure the operation completes.
typedef struct thread_struct thread_t;

// Globally declared queues.
extern atomic_queue_t   *input_queue;
extern atomic_queue_t	*work_queue;
extern atomic_queue_t	*output_queue;
extern atomic_queue_t	*run_queue;

extern sem_t            producer_done;
extern sem_t            consumer_done;
extern sem_t            consumed;

// Default constructor for the thread object. Requires a reference
// to the function to execute for p_thread instantiation.
thread_t *create_thread(void* (*)(void *));

// Default destructor for the thread struct.
void thread_delete(thread_t *);

// Implemented in executor.c. Acts as a constructor for all queues
// used in this implementation.
void make_queues(void);

// Implemented in executor.c. Generalized destructor to remove
// all created queue resources.
void destroy_queues(void);

// Single thread reader responsible for parsing I/O, constructing
// initial transform object and insertion to the input_queue.
void *reader(void *arg);

// 4 threads for producers instantiated from executor.c. This runnable
// is responsible for conducting the first series of transforms on
// the data in the input queue. Following operation inserts transform
// struct into work queue.
void *producer(void *arg);

// Master thread to oversee the consumer's process. Consumer thread
// count is dynamic so this thread regulates creation, sets flags for
// when a soft shutdown is necessary (when items in the work queue are
// below the lower threshold of work queue). Finished by shutting down
// each thread. The major scenarios are:
//      1) Producer not done and more items in queue than lower threshold
//				=> Spawn a single thread
//      2) Below thread capacity and upper bound is superseded
//              => Spawn a single thread
//      3) Producer not done and queue items are below lower threshold
//              => Start soft landing threads quickly
//      4) Producer finishes
//              => Create threads to limit and finish tasks quickly
void *monitor(void *arg);

// Thread function for extracting items from the work queue, applying
// second transform, reporting events to stderr, and insertion into
// the output queue. In the event the thread must be termination, its
// state value in the passed thread_t struct will be swapped from
// IS_RUNNING to IS_STOPPED. On the start of the next execution this
// loop will break and the thread stops.
void *consumer(void *arg);

// Outputs the data to stdin from the output queue.
void *writer(void *arg);

// Returns if the state of reporting updated. If it did, swaps the current
// state to the new one and returns true.
bool get_state(int, uint8_t *);

void report_state_change(void *, const uint8_t *);
