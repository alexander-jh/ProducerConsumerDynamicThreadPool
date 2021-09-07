#ifndef PROJECT1_THREADS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "atomic_queue.h"
#include "transform.h"
#include "transformMat.h"

#define PROJECT1_THREADS_H

extern atomic_queue_t   *input_queue;
extern atomic_queue_t	*work_queue;
extern atomic_queue_t	*output_queue;

extern bool             reader_done;
extern bool				producer_done;
extern bool				consumer_done;

extern uint16_t         writer_pos;

void *reader(void *arg);

void *producer(void *arg);

void *consumer(void *arg);

void *writer(void *arg);

#endif //PROJECT1_THREADS_H
