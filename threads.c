#include "threads.h"

struct transform_struct {
    char        cmd;
    int         queue_pos;
    double      encoded_ret,
                decoded_ret;
    int         work_queue_size;
    uint16_t    key,
                seq_num,
                encoded_key,
                decoded_key;
};

transform_t *transform_create() {
    transform_t *t = (transform_t *) malloc(sizeof(transform_t));
    if(t == NULL) {
        perror("Error: Failed to allocate memory for task_t.\n");
        exit(EXIT_FAILURE);
    }
    return t;
}

void task_destroy(transform_t *t) {
    free(t);
}

struct thread_struct {
	pthread_t       tid;
	atomic_int      state;
};

thread_t *create_thread(void* (*worker)(void *)) {
	pthread_attr_t attr;
	thread_t *t;
	t = (thread_t *) malloc(sizeof(thread_t));
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
}

void *reader(void *arg) {
	int writer_pos;
	char cmd;
	uint16_t key;
	transform_t *t;
    writer_pos = 0;
	// Using STDIN for file input
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
		if(cmd == 'X') {
            break;
		// If key is valid and cmd is valid create transform struct for
		// insertion.
        } else if(key > 0 && (cmd == 'A' || cmd == 'B' || cmd == 'C' ||
                              cmd == 'D' || cmd == 'E')) {
            t = transform_create();
            t->cmd = cmd;
            t->key = key;
            t->seq_num = ++writer_pos;
			atomic_queue_push(input_queue, t);
			produced++;
            written++;
		}
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	transform_t *task;
	int queue_pos;
	// Exits on completion of reader once the input_queue is empty.
	while(produced > 0 || !reader_done) {
		// Will automatically terminate on the end of all reader input
        if(!(task = (transform_t *) try_queue_pop(input_queue)))
            continue;
		switch(task->cmd) {
			case 'A':
                task->encoded_key = transformA1(task->cmd, &task->encoded_ret);
				break;
			case 'B':
                task->encoded_key = transformB1(task->cmd, &task->encoded_ret);
				break;
			case 'C':
                task->encoded_key = transformC1(task->cmd, &task->encoded_ret);
				break;
			case 'D':
                task->encoded_key = transformD1(task->cmd, &task->encoded_ret);
				break;
			case 'E':
                task->encoded_key = transformE1(task->cmd, &task->encoded_ret);
				break;
			default:
				break;
		}
        task->work_queue_size = atomic_queue_size(work_queue);
		queue_pos = atomic_queue_push(work_queue, task);
		task->queue_pos = queue_pos;
        current_task = (void *) task;
		produced--;
	}
	pthread_exit(arg);
}

void *monitor(void *arg) {
    consumer_start = time(NULL);
    while(produced > 0 || !reader_done)
        if(current_task)
            monitor_work((transform_t *) current_task);
    complete_consumption();
    pthread_exit(arg);
}

void monitor_work(transform_t *task) {
	int rq;
    uint16_t s;
	thread_t *t;
    s = state;
    if(get_state(task->work_queue_size)) {
        s = state;
        report_state_change(task, s);
    }
    rq = atomic_queue_size(run_queue);
    switch(s) {
        case BELOW_LOWER:
            // Kill threads
            if(rq && (t = (thread_t *) try_queue_pop(run_queue)))
                t->state = THREAD_STOPPING;
            break;
        case ABOVE_LOWER:
        case ABOVE_UPPER:
        case FULL_BUFFER:
            // Only triggers once for above lower
            if(((!rq) || (s != ABOVE_LOWER && rq < CONSUMER_THREAD_MAX)) &&
              ((t = create_thread(consumer)) != NULL))
                atomic_queue_push(run_queue, t);
            break;
        default:
            break;
    }
}

void complete_consumption() {
    thread_t *t;
    while(atomic_queue_size(run_queue) < BURN_THRESHOLD - 1 && written)
        if((t = create_thread(consumer)) != NULL)
            atomic_queue_push(run_queue, t);
    while(written)
        ;
    while(atomic_queue_size(run_queue))
        if((t = (thread_t *) try_queue_pop(run_queue)))
            t->state = THREAD_STOPPING;
    consumer_end = time(NULL);
}

void *consumer(void *arg) {
	transform_t *task;
	thread_t *self;
	// Get reference to thread struct.
	self = (thread_t *) arg;
	while(self->state != THREAD_STOPPING) {
        if(!(task = (transform_t *) try_queue_pop(work_queue)))
            continue;
		switch(task->cmd) {
			case 'A':
                task->decoded_key = transformA2(task->encoded_key, &task->decoded_ret);
				break;
			case 'B':
                task->decoded_key = transformB2(task->encoded_key, &task->decoded_ret);
				break;
			case 'C':
                task->decoded_key = transformC2(task->encoded_key, &task->decoded_ret);
				break;
			case 'D':
                task->decoded_key = transformD2(task->encoded_key, &task->decoded_ret);
				break;
			case 'E':
                task->decoded_key = transformE2(task->encoded_key, &task->decoded_ret);
				break;
			default:
                break;
		}
		atomic_queue_push(output_queue, task);
	}
	thread_delete(self);
	pthread_exit(NULL);
}

void *writer(void *arg) {
    heap_t *h;
	transform_t *t;
    int32_t exp_seq = 1;
    h = create_heap();
    while(written) {

        if((t = (transform_t *) try_queue_pop(output_queue)))
            insert(h, t, t->seq_num);
        else
            sleep(SLEEP_INTERVAL);

        if(minimum(h) == exp_seq) {
            t = (transform_t *) extract_min(h);
            fprintf(stdout, "%6d %6d %6c %6hu %23.1lf %6hu %23.1lf\n", t->seq_num,
                    t->queue_pos, t->cmd, t->encoded_key,
                    t->encoded_ret, t->decoded_key, t->decoded_ret);
            task_destroy(t);
            written--;
            exp_seq++;
        }
    }
    destroy_heap(h);
	pthread_exit(arg);
}

/**
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
bool get_state(int size) {
	uint16_t x = 0x0;
	x       = (!size)                    ? x | 0x8 : x & ~(0x1 << 0x3);
	x       = (size >  WORK_MIN_THRESH)  ? x | 0x4 : x & ~(0x1 << 0x2);
	x       = (size >  WORK_MAX_THRESH)  ? x | 0x2 : x & ~(0x1 << 0x1);
	x       = (size == WORK_BUFFER_SIZE) ? x | 0x1 : x & ~(0x1 << 0x0);
	if(x == state) return 0;
	state = x;
	return 1;
}

void report_state_change(void *data, uint16_t s) {
	transform_t *t = (transform_t *) data;
	switch(s) {
		case EMPTY_BUFFER:
			fprintf(stderr, "Work Queue Empty.\n");
			break;
		case BELOW_LOWER:
			fprintf(stderr,
					"\nWork Queue Below Lower: Item = %d  Cmd = %c   Key = %hu.\n",
					t->seq_num, t->cmd, t->key);
			break;
		case ABOVE_LOWER:
			fprintf(stderr,
			        "\nWork Queue Above Lower: Item = %d  Cmd = %c   Key = %hu.\n",
                    t->seq_num, t->cmd, t->key);
			break;
		case ABOVE_UPPER:
			fprintf(stderr,
			        "\nWork Queue Above Upper: Item = %d  Cmd = %c   Key = %hu.\n",
                    t->seq_num, t->cmd, t->key);
			break;
		case FULL_BUFFER:
			fprintf(stderr,
			        "\nWork Queue Full: Item = %d  Cmd = %c   Key = %hu.\n",
                    t->seq_num, t->cmd, t->key);
			break;
        default:
            break;
	}
}
