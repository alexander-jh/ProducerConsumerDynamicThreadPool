/*
 * Implementation of function and struct definitions in atomic_queue
 * header file.
 */

#include "atomic_queue.h"

struct atomic_queue_struct {
	size_t head, tail;
	pthread_mutex_t lock;
	sem_t front_sem, open_sem;
	uint16_t max;
    uint16_t size;
	void **data;
};

atomic_queue_t *atomic_queue_create(uint16_t size) {
	atomic_queue_t *q = calloc(1, sizeof(atomic_queue_t));
	if(!q) {
		perror("Error: memory allocation of atomic_queue_t failed.\n");
		exit(EXIT_FAILURE);
	}
	q->data = calloc(size, sizeof(void *));
	if(!q->data) {
		perror("Error: memory allocation of atomic_queue_t data failed.\n");
		exit(EXIT_FAILURE);
	}
	q->max = size;
    q->size = 0;
	sem_init(&q->front_sem, 0, 0);
	sem_init(&q->open_sem, 0, size);
	pthread_mutex_init(&q->lock, NULL);
	return q;
}

void atomic_queue_destroy(atomic_queue_t *q) {
	free(q->data);
	free(q);
}

void atomic_queue_add(atomic_queue_t *q, void *ele) {
	sem_wait(&q->open_sem);
	pthread_mutex_lock(&q->lock);
	q->data[q->head++] = ele;
    q->size++;
	if(q->head >= q->max)
		q->head = 0;
	pthread_mutex_unlock(&q->lock);
	sem_post(&q->front_sem);
}

void *atomic_queue_remove(atomic_queue_t *q, bool is_wq) {
	sem_wait(&q->front_sem);
	return _atomic_queue_remove(q, is_wq);
}

void *_atomic_queue_remove(atomic_queue_t *q, bool is_wq) {
	uint16_t pos;
	pthread_mutex_lock(&q->lock);
	void *pop = q->data[q->tail++];
	pos = q->tail;
    q->size--;
	if(q->tail >= q->max) q->tail = 0;
	if(is_wq) set_queue_pos((transform_t *) pop, pos);
	pthread_mutex_unlock(&q->lock);
	sem_post(&q->open_sem);
	return pop;
}

void *_atomic_queue_try_remove(atomic_queue_t *q, bool is_wq) {
	return !sem_trywait(&q->front_sem) ?
	       _atomic_queue_remove(q, is_wq) :
		   NULL;
}
