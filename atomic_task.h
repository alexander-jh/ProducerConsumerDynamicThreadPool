#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define MPANIC(x) ;     assert(x != NULL)

/**
 * Typedef'd struct to track the current status of each major
 * thread task atomically through mutex locks.
 *  status  - boolean type to signify if task is completed
 *  mutex   - mutex lock
 */
typedef struct task_struct task_t;

/**
 * Standard task constructor.
 * @return  - reference to task_t
 */
task_t *task_create(void);

/**
 * Standard task destructor.
 * @param   - reference to task_t
 */
void task_destroy(task_t *);

/**
 * Locks mutex and sets the current task status to false
 * signifying the task is now complete.
 * @param   - reference to task_t
 */
void task_set_complete(task_t *);

/**
 * Informs caller is the current task is still running.
 * @param   - reference to task_t
 * @return  - returns task_t->status
 */
bool task_is_complete(task_t *);