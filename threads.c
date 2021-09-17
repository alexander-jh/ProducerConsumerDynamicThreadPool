#include "threads.h"

struct thread_struct {
	pthread_t       tid;
	atomic_int      state;
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
	pthread_create(&t->tid, &attr, worker, (void *) t);
	return t;
}

void thread_delete(thread_t * t) {
	free(t);
	t = NULL;
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
			sem_post(&consumed);
		}
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	// Exits on completion of reader once the input_queue is empty.
	while(atomic_queue_size(input_queue)) {
		// Will automatically terminate on the end of all reader input
		if(!(task = (transform_t *) try_queue_pop(input_queue)))
			continue;
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
	int rq;
	thread_t *t;
	uint8_t state = EMPTY_BUFFER;
	// Exits on semaphore signal from producer
	while(sem_trywait(&producer_done) < 0) {
		// Update current state and report any potential changes
		if(get_state(atomic_queue_size(work_queue), &state))
			report_state_change(atomic_queue_top(work_queue), &state);

		rq = atomic_queue_size(run_queue);

		switch(state) {
			case BELOW_LOWER:
				// Kill threads
				t = (thread_t *) try_queue_pop(run_queue);
				if(t) t->state = THREAD_STOPPING;
				break;
			case ABOVE_LOWER:
			case ABOVE_UPPER:
			case FULL_BUFFER:
				// Only triggers once for above lower
				if((!rq) || (state != ABOVE_LOWER && rq < CONSUMER_THREAD_MAX)) {
					t = create_thread(consumer);
					atomic_queue_push(run_queue, t, false);
					sleep(SLEEP_INTERVAL);
				}
				break;
			default:
				break;
		}
	}

	// Spawns max number of threads to consume remaining elements.
	while(atomic_queue_size(run_queue) < CONSUMER_THREAD_MAX) {
		atomic_queue_push(run_queue, create_thread(consumer), false);
		sleep(1);
	}
	// Wait for work queue to dry out
	while(atomic_queue_size(work_queue));
	// Soft kill each thread
	while(atomic_queue_size(run_queue)) {
		t = atomic_queue_pop(run_queue);
		t->state = THREAD_STOPPING;
	}
	// Wait until last element is posted to output
	while(sem_getvalue(&consumed, &rq) != 0);

	pthread_exit(arg);
}

void *consumer(void *arg) {
	transform_t *task;
	thread_t *self;
	uint16_t out_val;
	double retval;
	// Get reference to thread struct.
	self = (thread_t *) arg;
	while(self->state != THREAD_STOPPING) {
		task = (transform_t *) try_queue_pop(work_queue);
		// Kills thread after exhaustion of queue
		if(!task) continue;
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
		atomic_queue_push(output_queue, task, false);
		sem_trywait(&consumed);
	}
	thread_delete(self);
	pthread_exit(NULL);
}

void *writer(void *arg) {
	int sem_v;
	transform_t *t;
    while(1) {
		// Redundant check to verify item still exists in output queue
		// if it does not will get stuck in a semaphore wait indefinitely.
	    if((t = (transform_t *) try_queue_pop(output_queue))) {
		    printf("%d %d %c %hu %lf %hu %lf\n", get_seq_num(t),
		           get_queue_pos(t), get_cmd(t), get_encoded_key(t),
		           get_encoded_ret(t), get_decoded_key(t), get_decoded_ret(t));
		    task_destroy(t);
	    }
	    sem_getvalue(&consumer_done, &sem_v);
		if(sem_v == 1 && !atomic_queue_size(output_queue)) break;
    }
	pthread_exit(arg);
}

/*
 * This is abstract but stay with me. Let E, L, H, F represent the truth
 * of a boolean expression and W = |work_queue| such that:
 *      E   :=  W > 0               (empty)
 *      L   :=  W >= MIN_THRESH     (at least 50)
 *      H   :=  W >= MAX_THRESH     (at least 150)
 *      F   :=  W == BUFFER_MAX     (full)
 * We make the pair into a binary string so:
 *      ELHF <=> 1111   and   E'L'H'F' <=> 0000
 * Now, there are five major states with transitions S_i with i IN {1,..,5}
 * and each state can be represented by the binary strings:
 *      S_1 :=  EMPTY_BUFFER    :=  0x8 :=  EL'H'F'     := 1000
 *      S_2 :=  BELOW_LOWER     :=  0x0 :=  E'L'H'F'    := 0000
 *      S_3 :=  ABOVE_LOWER     :=  0x4 :=  E'LH'F'     := 0100
 *      S_4 :=  ABOVE_UPPER     :=  0x6 :=  E'LHF'      := 0110
 *      S_5 :=  FULL_BUFFER     :=  0x7 :=  E'LHF       := 0111
 * Since with how the bitwise operations are defined, the four ternaries
 * will always yield one of the five states. If it changes it can be
 * reported from the calling thread.
 */
bool get_state(int size, uint8_t *last) {
	uint8_t x = 0x0;
	x       = (!size)                    ? x | 8 : x & ~(1UL << 3);
	x       = (size >  WORK_MIN_THRESH)  ? x | 4 : x & ~(1UL << 2);
	x       = (size >  WORK_MAX_THRESH)  ? x | 2 : x & ~(1UL << 1);
	x       = (size == WORK_BUFFER_SIZE) ? x | 1 : x & ~(1UL << 0);
	if(x == *last) return 0;
	*last = x;
	return 1;
}

void report_state_change(void *data, const uint8_t *state) {
	transform_t *t = (transform_t *) data;
	switch(*state) {
		case EMPTY_BUFFER:
			fprintf(stderr, "Work Queue Empty.\n");
			break;
		case BELOW_LOWER:
			fprintf(stderr,
					"\nWork Queue Below Lower: Item = %d  Cmd = %c   Key = %hu.\n",
					get_seq_num(t), get_cmd(t), get_key(t));
			break;
		case ABOVE_LOWER:
			fprintf(stderr,
			        "\nWork Queue Above Lower: Item = %d  Cmd = %c   Key = %hu.\n",
			        get_seq_num(t), get_cmd(t), get_key(t));
			break;
		case ABOVE_UPPER:
			fprintf(stderr,
			        "\nWork Queue Above Upper: Item = %d  Cmd = %c   Key = %hu.\n",
			        get_seq_num(t), get_cmd(t), get_key(t));
			break;
		case FULL_BUFFER:
			fprintf(stderr,
			        "\nWork Queue Full: Item = %d  Cmd = %c   Key = %hu.\n",
			        get_seq_num(t), get_cmd(t), get_key(t));
			break;
	}
}
