#include "threads.h"

struct thread_struct {
	pthread_t       tid;
	pthread_mutex_t mutex;
	int             state;
};

thread_t *create_thread(void* (*worker)(void *)) {
	pthread_attr_t attr;
	thread_t *t;
	t = malloc(sizeof(thread_t));
	if(!t) {
		fprintf(stderr, "Error: Failed to allocate thread.\n");
		exit(EXIT_FAILURE);
	}
	pthread_attr_init(&attr);
	// Create detached thread so join is not necessary
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	t->state = THREAD_RUNNING;
	pthread_mutex_init(&t->mutex, NULL);
	pthread_create(&t->tid, &attr, worker, (void *) t);
	return t;
}

void thread_delete(thread_t * t) {
	pthread_mutex_destroy(&t->mutex);
	free(t);
}

void *reader(void *arg) {
	int writer_pos;
	char cmd;
	uint16_t key;
	transform_t *t;
    writer_pos = 0;
	/* Using STDIN for file input */
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
		if(cmd == 'X') {
            break;
		// If key is valid and cmd is valid create transform struct for
		// insertion.
        } else if(key > 0 && (cmd == 'A' || cmd == 'B' || cmd == 'C' ||
                              cmd == 'D' || cmd == 'E')) {
			t = transform_create();
			set_cmd(t, cmd);
			set_key(t, key);
			set_seq_num(t, ++writer_pos);
			atomic_queue_push(input_queue, t, false);
			total_produced++;
		}
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	// Exits on completion of reader once the input_queue is empty.
	while(!reader_done || atomic_queue_size(input_queue)) {
		task = (transform_t *) atomic_queue_pop(input_queue);
		switch(get_cmd(task)) {
			case 'A':
				out_val = transformA1(get_key(task), &retval);
				break;
			case 'B':
				out_val = transformB1(get_key(task), &retval);
				break;
			case 'C':
				out_val = transformC1(get_key(task), &retval);
				break;
			case 'D':
				out_val = transformD1(get_key(task), &retval);
				break;
			case 'E':
				out_val = transformE1(get_key(task), &retval);
				break;
			default:
				continue;
		}
		set_encoded_key(task, out_val);
		set_encoded_ret(task, retval);
		atomic_queue_push(work_queue, task, true);
	}
	pthread_exit(arg);
}

void *monitor(void *arg) {
	int wq, rq, last;
	thread_t *t;
	last = EMPTY_BUFFER;
	while(1) {
		wq = atomic_queue_size(work_queue);
		rq = atomic_queue_size(run_queue);
		// Check reportable events.
		if(!alert_waiting) last = set_alert(last, wq);
		// Create threads and sleep after creation.
		if((rq < CONSUMER_THREAD_MAX) && ((wq > WORK_MIN_THRESH && rq < 1) ||
				(wq > WORK_MAX_THRESH) || (wq && producer_done))) {
			t = create_thread(consumer);
			atomic_queue_push(run_queue, t, false);
			sleep(SLEEP_INTERVAL);
		// If there are more than one thread, the producer is alive, and
		// the work queue is below the minimum threshold cancel threads.
		} else if(rq && wq < WORK_MIN_THRESH && !producer_done) {
			t = (thread_t *) atomic_queue_pop(run_queue);
			pthread_mutex_lock(&t->mutex);
			t->state = THREAD_STOPPING;
			pthread_mutex_unlock(&t->mutex);
		// Exit loop once work queue is empties and producer is done.
		} else if(!atomic_queue_size(work_queue) && producer_done) {
			break;
		}
	}
	// Kill all remaining consumer threads.
	while(atomic_queue_size(run_queue)) {
		t = (thread_t *) atomic_queue_pop(run_queue);
		pthread_mutex_lock(&t->mutex);
		t->state = THREAD_STOPPING;
		pthread_mutex_unlock(&t->mutex);
	}
	// Spinlock until remaining consumers finish
	while(total_produced != total_consumed);
	
	pthread_exit(arg);
}

void *consumer(void *arg) {
	transform_t *task;
	thread_t *self;
	uint16_t out_val;
	double retval;
	// Get reference to thread struct.
	self = (thread_t *) arg;
	while(1) {
		// Acquire lock to check current state no other exclusion is
		// necessary throughout this work cycle.
		pthread_mutex_lock(&self->mutex);
		if(self->state == THREAD_STOPPING) {
			pthread_mutex_unlock(&self->mutex);
			break;
		}
		pthread_mutex_unlock(&self->mutex);

		task = (transform_t *) atomic_queue_pop(work_queue);

		// If alert exists report.
		if(alert_waiting) {
			alert_waiting = 0;
			semaphore_alert(get_seq_num(task),
			                get_cmd(task),
			                get_key(task));
		}
		switch(get_cmd(task)) {
			case 'A':
				out_val = transformA2(get_encoded_key(task), &retval);
				break;
			case 'B':
				out_val = transformB2(get_encoded_key(task), &retval);
				break;
			case 'C':
				out_val = transformC2(get_encoded_key(task), &retval);
				break;
			case 'D':
				out_val = transformD2(get_encoded_key(task), &retval);
				break;
			case 'E':
				out_val = transformE2(get_encoded_key(task), &retval);
				break;
			default:
				continue;
		}
		set_decoded_key(task, out_val);
		set_decoded_ret(task, retval);
		// Mutual exclusion guaranteed in queue structure
		if(task != NULL) total_consumed++;
		atomic_queue_push(output_queue, task, false);
	}
	thread_delete(self);
	pthread_exit(NULL);
}

void *writer(void *arg) {
	transform_t *t;
    while(atomic_queue_size(output_queue) || !consumer_done) {
		// Redundant check to verify item still exists in output queue
		// if it does not will get stuck in a semaphore wait indefinitely.
	    if(atomic_queue_size(output_queue)) {
		    t = (transform_t *) atomic_queue_pop(output_queue);
		    printf("%d %d %c %hu %lf %hu %lf\n", get_seq_num(t),
		           get_queue_pos(t), get_cmd(t), get_encoded_key(t),
		           get_encoded_ret(t), get_decoded_key(t), get_decoded_ret(t));
		    task_destroy(t);
	    }
    }
	pthread_exit(arg);
}

int set_alert(int last, int wq) {
	int current_state;
	if(wq == WORK_BUFFER_SIZE)
		current_state = FULL_BUFFER;
	else if(wq == 0)
		current_state = EMPTY_BUFFER;
	else if(wq > WORK_MAX_THRESH)
		current_state = ABOVE_UPPER;
	else if(wq > WORK_MIN_THRESH && wq < WORK_MAX_THRESH)
		current_state = ABOVE_LOWER;
	else
		current_state = BELOW_LOWER;

	if(current_state != last) {
		last = current_state;

		switch(current_state) {
			case FULL_BUFFER:
				sem_post(&full_buffer);
				break;
			case EMPTY_BUFFER:
				sem_post(&empty_buffer);
				break;
			case ABOVE_UPPER:
				sem_post(&upper_thresh);
				break;
			case ABOVE_LOWER:
				sem_post(&lower_thresh);
				break;
			default:
				break;
		}
		alert_waiting = (last != BELOW_LOWER);
	}
	return last;
}

void semaphore_alert(int pos, char cmd, uint16_t key) {
	if (sem_trywait(&lower_thresh) == 0) {
		fprintf(stderr,
				"Lower Threshold Crossed: %d %c %hu\n",
				pos, cmd, key);
		alert_waiting = 0;
	} else if (sem_trywait(&upper_thresh) == 0) {
		fprintf(stderr,
		        "Upper Threshold Crossed: %d %c %hu\n",
		        pos, cmd, key);
		alert_waiting = 0;
	} else if(sem_trywait(&full_buffer) == 0) {
		fprintf(stderr,
		        "Buffer Full: %d %c %hu\n",
		        pos, cmd, key);
		alert_waiting = 0;
	} else if(sem_trywait(&empty_buffer) == 0) {
		fprintf(stderr,
		        "Buffer Empty: %d %c %hu\n",
		        pos, cmd, key);
	}
}
