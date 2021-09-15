/*
 * Implementation of function and struct definitions in atomic_queue
 * header file.
 */

#include "atomic_queue.h"

struct atomic_queue_struct {
	void                **data;
	pthread_mutex_t     mutex;
	int                 size,
						capacity,
						back,
						front;
	sem_t               empty,
		                full;
};

atomic_queue_t *atomic_queue_create(int capacity, size_t data_p) {
	atomic_queue_t *q = calloc(sizeof(atomic_queue_t), 1);
	if(!q) {
		fprintf(stderr, "Error: Failed to allocate queue.\n");
		exit(EXIT_FAILURE);
	}
	q->data = calloc(data_p, capacity);
	if(!q->data) {
		fprintf(stderr, "Error: Failed to allocate queue.\n");
		exit(EXIT_FAILURE);
	}
	q->size = 0;
	q->capacity = capacity;
	q->front = 0;
	q->back = 0;
	pthread_mutex_init(&q->mutex, NULL);
	sem_init(&q->empty, 0, 0);
	sem_init(&q->full, 0, capacity);
	return q;
}

void atomic_queue_destroy(atomic_queue_t *q) {
	free(q->data);
	q->data = NULL;
	free(q);
	q = NULL;
}

void _atomic_queue_push(atomic_queue_t *q, void *data, bool is_wq) {
	q->data[q->back++] = data;
	if(is_wq) set_queue_pos(q->data[q->back - 1], q->back - 1);
	q->back %= q->capacity;
	q->size++;
}

void atomic_queue_push(atomic_queue_t *q, void *data, bool is_wq) {
	sem_wait(&q->full);
	pthread_mutex_lock(&q->mutex);
	_atomic_queue_push(q, data, is_wq);
	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->empty);
}

void *_atomic_queue_pop(atomic_queue_t *q) {
	void *data = q->data[q->front++];
	q->front %=  q->capacity;
	q->size--;
	return data;
}

void *atomic_queue_pop(atomic_queue_t *q) {
	void *data;
	sem_wait(&q->empty);
	pthread_mutex_lock(&q->mutex);
	data = _atomic_queue_pop(q);
	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->full);
	return data;
}

int atomic_queue_size(atomic_queue_t *q) {
	int size;
	pthread_mutex_lock(&q->mutex);
	size = q->size;
	pthread_mutex_unlock(&q->mutex);
	return size;
}