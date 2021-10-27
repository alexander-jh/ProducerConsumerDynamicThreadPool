#include "atomic_thread.h"

struct thread_struct {
	pthread_t           tid;
	bool                state;
    pthread_mutex_t     mutex;
};

thread_t *thread_create(void* (*worker)(void *)) {
	thread_t *t;
	t = (thread_t *) malloc(sizeof(thread_t));
	if(!t) {
		fprintf(stderr, "Error: Failed to allocate thread.\n");
		exit(EXIT_FAILURE);
	}
	t->state = true;
    pthread_mutex_init(&t->mutex, NULL);
	pthread_create(&t->tid, NULL, worker, (void *) t);
	return t;
}

void thread_delete(thread_t * t) {
    pthread_mutex_destroy(&t->mutex);
	free(t);
}

void thread_stop(thread_t *t) {
    pthread_mutex_lock(&t->mutex);
    t->state = false;
    pthread_mutex_unlock(&t->mutex);
    if(t->tid) pthread_join(t->tid, NULL);
    thread_delete(t);
}

bool thread_running(thread_t *t) {
    bool status;
    pthread_mutex_lock(&t->mutex);
    status = t->state;
    pthread_mutex_unlock(&t->mutex);
    return status;
}
