#include "threads.h"

struct thread_struct {
	pthread_t       tid;
	pthread_mutex_t mutex;
	int             state;
};

struct flag_struct {
	atomic_int      above_low;
	atomic_int      above_high;
	atomic_int      is_full;
	atomic_int      is_empty;
	atomic_int      state;
};

flag_t *create_flags() {
	flag_t *flag = malloc(sizeof(flag_t));
	if(!flag) {
		flags->above_low = 0;
		flags->above_high = 0;
		flags->is_full = 0;
		flags->is_empty = 1;
		flags->state = WORK_QUEUE_EMPTY;
	}
	return flag;
}

void flag_delete(flag_t *f) {
	free(f);
	f = NULL;
}

thread_t *create_thread(void* (*worker)(void *)) {
	pthread_attr_t attr;
	thread_t *t;
	t = malloc(sizeof(thread_t));
	pthread_attr_init(&attr);
	// Create detached thread so join is not necessary
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	t->state = THREAD_RUNNING;
	pthread_mutex_init(&t->mutex, NULL);
	pthread_create(&t->tid, &attr, worker, (void *) t);
	return t;
}

void thread_delete(thread_t * t) {
	free(t);
}

void *reader(void *arg) {
	int ret;
	char cmd;
	uint16_t key;
	transform_t *t = transform_create();
    /* Using STDIN for file input */
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
		if(cmd == 'X') {
            break;
        } else if(key > 0 && (cmd == 'A' || cmd == 'B' || cmd == 'C' ||
                              cmd == 'D' || cmd == 'E')) {
			set_cmd(t, cmd);
			set_key(t, key);
			set_seq_num(t, ++writer_pos);
			CHECK_IF_INPUT_FULL:
			ret = atomic_queue_push(input_queue, t);
			if(!ret) {
				sleep(SLEEP_INTERVAL);
				goto CHECK_IF_INPUT_FULL;
			}
			t = transform_create();
		}
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	int ret;
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!reader_done || atomic_queue_size(input_queue) > 0) {
		task = (transform_t *) atomic_queue_pop(input_queue, false);

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
		CHECK_IF_WORK_FULL:
			ret = atomic_queue_push(work_queue, task);
			if(!ret) {
				sleep(SLEEP_INTERVAL);
				goto CHECK_IF_WORK_FULL;
			}
	}
	pthread_exit(arg);
}

void *monitor(void *arg) {
	int wq, rq;
	thread_t *t;

	while(1) {
		sleep(SLEEP_INTERVAL);
		wq = atomic_queue_size(work_queue);
		rq = atomic_queue_size(run_queue);
		if ((wq > WORK_MIN_THRESH && rq < 1) ||
            (rq < CONSUMER_THREAD_MAX && wq > WORK_MAX_THRESH) ||
			(rq < 1 && producer_done)) {
			t = create_thread(consumer);
			atomic_queue_push(run_queue, t);
		} else if((rq > 1 && wq < WORK_MAX_THRESH) ||
				(rq && wq < WORK_MIN_THRESH && !producer_done)) {
			t = (thread_t *) atomic_queue_pop(run_queue, false);
			if(!t) continue;
			pthread_mutex_lock(&t->mutex);
			t->state = THREAD_STOPPING;
			pthread_mutex_unlock(&t->mutex);
		} else if(wq == 0 && producer_done) {
			break;
		}
	}

	while(atomic_queue_size(run_queue)) {
		t = (thread_t *) atomic_queue_pop(run_queue, false);
		if(!t) continue;
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
	int ret;

	self = (thread_t *) arg;

	while(1) {
		pthread_mutex_lock(&self->mutex);

		if(self->state == THREAD_STOPPING) {
			pthread_mutex_unlock(&self->mutex);
			break;
		}

		if(!(task = (transform_t *) atomic_queue_pop(work_queue, true))) {
            pthread_mutex_unlock(&self->mutex);
            continue;
        }

		report_status(atomic_queue_size(work_queue),
		              get_cmd(task),
		              get_queue_pos(task),
		              get_key(task));

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

		CHECK_IF_OUTPUT_FULL:
			ret = atomic_queue_push(output_queue, task);
			if(!ret) {
				pthread_mutex_unlock(&self->mutex);
				sleep(SLEEP_INTERVAL);
				pthread_mutex_lock(&self->mutex);
				goto CHECK_IF_OUTPUT_FULL;
			}
		pthread_mutex_unlock(&self->mutex);
	}
	thread_delete(self);
	pthread_exit(NULL);
}

void *writer(void *arg) {
	transform_t *t;
    while(1) {
        t = (transform_t *) atomic_queue_pop(output_queue, false);
        if(atomic_queue_size(output_queue) == 0 && consumer_done)
            pthread_exit(arg);
        else if(t) {
	        printf("%d %d %c %hu %lf %hu %lf\n", get_seq_num(t),
	               get_queue_pos(t), get_cmd(t), get_encoded_key(t),
	               get_encoded_ret(t), get_decoded_key(t), get_decoded_ret(t));
	        task_destroy(t);
        }
    }
}

void update_flag_status(int size) {
	flags->above_low    = size > WORK_MIN_THRESH;
	flags->above_high   = size > WORK_MAX_THRESH;
	flags->is_full      = size == WORK_BUFFER_SIZE;
	flags->is_empty     = size == 0;
}

void report_status(int size, char cmd, int pos, uint16_t key) {
	update_flag_status(size);
	int current_state;
	if(flags->is_empty) {
		current_state = WORK_QUEUE_EMPTY;
		if(flags->state != WORK_QUEUE_EMPTY)
			fprintf(stderr, "Empty: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	} else if(!flags->above_low && !flags->is_empty) {
		current_state = BELOW_LOWER_BOUND;
		if(flags->state == ABOVE_LOWER_BOUND)
			fprintf(stderr, "Below Lower: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	} else if(flags->above_high && !flags->is_full) {
		current_state = ABOVE_HIGHER_BOUND;
		if(flags->state == BELOW_HIGHER_BOUND)
			fprintf(stderr, "Above Upper: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	} else if(flags->is_full) {
		current_state = WORK_QUEUE_FULL;
		if(flags->state != WORK_QUEUE_FULL)
			fprintf(stderr, "Full: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	} else if(flags->above_low && !flags->above_high) {
		current_state = ABOVE_LOWER_BOUND;
		if(flags->state == BELOW_LOWER_BOUND)
			fprintf(stderr, "Above Lower: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	} else {
		current_state = BELOW_HIGHER_BOUND;
		if(flags->state == ABOVE_HIGHER_BOUND)
			fprintf(stderr, "Above Higher: %c %d %hu\n",
			        cmd, pos, key);
		flags->state = current_state;
	}
}
