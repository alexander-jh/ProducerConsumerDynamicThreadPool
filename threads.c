#include "threads.h"

struct thread_struct {
	pthread_t       tid;
	pthread_mutex_t mutex;
	uint16_t        state;
	atomic_queue_t  *queue;
};

thread_t *create_thread(atomic_queue_t *q, void* (*worker)(void *)) {
	pthread_attr_t attr;
	thread_t *t = (thread_t *) calloc(sizeof(thread_t *), 1);
	if(t == NULL) return NULL;
	pthread_attr_init(&attr);
	// Create detached thread so join is not necessary
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	t->queue = q;
	t->state = THREAD_RUNNING;
	pthread_mutex_init(&t->mutex, NULL);
	pthread_create(&t->tid, &attr, worker, (void *) t);
	pthread_attr_destroy(&attr);
	return t;
}

void thread_delete(thread_t * t) {
	free(t);
}

void *reader(void *arg) {
	char cmd;
	uint16_t key;
	transform_t *t = transform_create();
    /* Using STDIN for file input */
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
		if(cmd == 'X') {
            break;
        } else {
			set_cmd(t, cmd);
			set_key(t, key);
			set_seq_num(t, ++writer_pos);
			atomic_queue_add(input_queue, t);
			t = transform_create();
		}
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!reader_done || atomic_queue_size(input_queue)) {

		task = (transform_t *) atomic_queue_remove(input_queue, false);

		if(task == NULL) continue;

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
		atomic_queue_add(work_queue, task);
	}
	pthread_exit(arg);
}

void *monitor(void *arg) {
	int wq, rq;
	atomic_queue_t *run_queue;
	thread_t *t;

	run_queue = atomic_queue_create(CONSUMER_THREAD_MAX);

	while(1) {
		sleep(SLEEP_INTERVAL);
		wq = atomic_queue_size(work_queue);
		rq = atomic_queue_size(run_queue);
		if ((wq > WORK_MIN_THRESH && rq < 1) ||
				(rq < CONSUMER_THREAD_MAX && wq > WORK_MAX_THRESH)) {
			printf("Consumer working \n");
			atomic_queue_add(run_queue, create_thread(work_queue, consumer));
		} else if((rq > 1 && wq < WORK_MAX_THRESH) ||
				(rq && wq < WORK_MIN_THRESH && !producer_done)) {
			t = (thread_t *) atomic_queue_remove(run_queue, false);
			pthread_mutex_lock(&t->mutex);
			t->state = THREAD_STOPPING;
			pthread_mutex_unlock(&t->mutex);
		} else if(!wq && producer_done) {
			break;
		}
	}

	while(atomic_queue_size(run_queue)) {
		t = (thread_t *) atomic_queue_remove(run_queue, false);
		pthread_mutex_lock(&t->mutex);
		t->state = THREAD_STOPPING;
		pthread_mutex_unlock(&t->mutex);
	}

	pthread_exit(arg);
}

void *consumer(void *arg) {
	transform_t *task;
	thread_t *self;
	uint16_t out_val;
	double retval;

	printf("Consumer working \n");

	self = (thread_t *) arg;

	while(1) {
		printf("Consumer working \n");
		pthread_mutex_lock(&self->mutex);

		if(self->state == THREAD_STOPPING) {
			pthread_mutex_unlock(&self->mutex);
			break;
		}

		if(!(task = (transform_t *) atomic_queue_remove(self->queue, true)))
			continue;

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
		atomic_queue_add(output_queue, task);

		pthread_mutex_unlock(&self->mutex);
	}
	thread_delete(self);
	pthread_exit(NULL);
}

void *writer(void *arg) {
	transform_t *t;
    while(true) {
        t = (transform_t *) atomic_queue_remove(output_queue, false);
        if(atomic_queue_size(output_queue) == 0 && consumer_done)
            pthread_exit(arg);
        else if(t != NULL) {
	        printf("%hu %d %c %hu %lf %hu %lf\n", get_seq_num(t),
	               get_queue_pos(t), get_cmd(t), get_encoded_key(t),
	               get_encoded_ret(t), get_decoded_key(t), get_decoded_ret(t));
	        task_destroy(t);
        }
    }
}
