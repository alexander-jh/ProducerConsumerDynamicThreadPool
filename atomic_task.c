#include "atomic_task.h"

struct task_struct {
    bool                status;
    pthread_mutex_t     mutex;
};

task_t *task_create() {
    task_t *t = (task_t *) malloc(sizeof(task_t)) MPANIC(t);
    t->status = false;
    pthread_mutex_init(&t->mutex, NULL);
    return t;
}

void task_destroy(task_t *t) {
    if(t) {
        pthread_mutex_destroy(&t->mutex);
        free(t);
    }
}

void task_set_complete(task_t *t) {
    pthread_mutex_lock(&t->mutex);
    t->status = true;
    pthread_mutex_unlock(&t->mutex);
}

bool task_is_complete(task_t *t) {
    bool status;
    pthread_mutex_lock(&t->mutex);
    status = t->status;
    pthread_mutex_unlock(&t->mutex);
    return status;
}