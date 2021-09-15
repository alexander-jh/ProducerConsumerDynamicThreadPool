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

#define FULL_BUFFER             0
#define EMPTY_BUFFER            1
#define ABOVE_LOWER             2
#define ABOVE_UPPER             3
#define BELOW_LOWER             4

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

// Globally declared boolean flags for completion of each general
// worker class. Atomic integer type is used to ensure reads and
// writes will not be interrupted.
extern atomic_int       reader_done;
extern atomic_int		producer_done;
extern atomic_int		consumer_done;
extern atomic_int       alert_waiting;
extern atomic_int       total_produced;
extern atomic_int       total_consumed;

// Semaphores used to signal reporting of necessary information to
// the consumer depending on current required action.
extern sem_t            lower_thresh;
extern sem_t            upper_thresh;
extern sem_t            full_buffer;
extern sem_t            empty_buffer;

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

// Implemented in executor.c. Generalized constructor for all
// semaphores.
void make_semaphores(void);

// Implemented in executor.c. Generalized destructor for all
// semaphores.
void destroy_semaphores(void);

// Implemented in executor.c. Generalized constructor for default
// values of the boolean variables.
void set_booleans(void);

// Called by consumer thread function. If the alert_waiting flag
// is raised. Checks for the raised semaphore to report the
// event type to stderr.
void semaphore_alert(int pos, char cmd, uint16_t key);

// Called by the monitor thread function. Compares current state
// of the work_queue to the previous to resolve if a reportable
// event occured. If it does, sets the alert_waiting flag and
// posts to the corresponding semaphore.
int set_alert(int last, int wq);

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
