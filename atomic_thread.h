#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Struct for containing the consumer threads. This is necessary
 * to have a shared memory space so the monitor thread can softly
 * shut down threads and ensure the operation completes.
 */
typedef struct thread_struct thread_t;

/**
 * Default constructor for the thread object. Requires a reference
 * to the function to execute for p_thread instantiation.
 * @param   - reference to thread safe function
 * @return  - pthread instantiated to referenced function
 */
thread_t *thread_create(void* (*)(void *));

/**
 * Default destructor for the thread struct.
 * @param   - reference to thread struct
 */
void thread_delete(thread_t *);

/**
 * Atomically updates state of the current thread to stop.
 * @param   - reference to thread_t
 */
void thread_stop(thread_t *);

/**
 * Atomically reports to caller the current status of the thread.
 * @return  - boolean signifying if the thread is still active
 */
bool thread_running(thread_t *);
