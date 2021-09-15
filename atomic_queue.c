/*
 * Implementation of function and struct definitions in atomic_queue
 * header file.
 */

#include "atomic_queue.h"

struct queue_node_struct {
	void            *data;
	queue_node_t    *next;
};

struct atomic_queue_struct {
	int                 size;
	int                 max;
	pthread_mutex_t     *lock;
	queue_node_t        *head;
	queue_node_t        *tail;
};

atomic_queue_t *atomic_queue_create(int size) {
	atomic_queue_t *q = malloc(sizeof(atomic_queue_t));
	if(!q) {
		perror("Error: memory allocation of atomic_queue_t failed.\n");
		exit(EXIT_FAILURE);
	}
	q->head = NULL;
	q->tail = NULL;
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	if(!mutex) {
		perror("Error: memory allocation of atomic_queue_t failed.\n");
		exit(EXIT_FAILURE);
	}
	q->lock = mutex;
	q->size = 0;
	q->max = size;
	return q;
}

void atomic_queue_destroy(atomic_queue_t *queue) {
	free(queue->lock);
	free(queue);
	queue = NULL;
}

bool atomic_queue_push(atomic_queue_t *queue, void *data, bool is_wq) {
	bool ret = true;
	queue_node_t *element, *tail;
	element = malloc(sizeof(queue_node_t));
	element->data = data;
	element->next = NULL;

	pthread_mutex_lock(queue->lock);
	if(is_wq) set_queue_pos(element->data, queue->size);
	if(queue->head == NULL) {
		queue->head = element;
		queue->tail = element;
		++queue->size;
	} else if(queue->size == queue->max) {
		ret = false;
	} else {
		tail = queue->tail;
		tail->next = element;
		queue->tail = element;
		++queue->size;
	}
	pthread_mutex_unlock(queue->lock);
	return ret;
}

void *atomic_queue_pop(atomic_queue_t *queue) {
	void *data;
	queue_node_t *head;
	pthread_mutex_lock(queue->lock);
	head = queue->head;
	if(!head) {
		data = NULL;
	} else {
		queue->head = head->next;
		data = head->data;
		free(head);
		--queue->size;
	}
	pthread_mutex_unlock(queue->lock);
	return data;
}

int atomic_queue_size(atomic_queue_t *queue) {
	int size;
	pthread_mutex_lock(queue->lock);
	size = queue->size;
	pthread_mutex_unlock(queue->lock);
	return size;
}
