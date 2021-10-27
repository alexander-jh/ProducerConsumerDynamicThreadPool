/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include "executor.h"

/**
 * Entry function for main program.
 */
int main(void) {
    /* Declare counter, timers and thread pools. */
	int i;
	pthread_t   reader_thread,
				writer_thread,
                monitor_thread,
				producer_threads[PRODUCER_THREADS];
	/* Instantiate queues and other variables. */
	make_queues();

	/* Mark the start of the program */
	start = time(NULL);

    /* Create reader thread. */
	pthread_create(&reader_thread, NULL, reader, NULL);

    /* Create producer threads. */
	producer_start = time(NULL);
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    pthread_create(&monitor_thread, NULL, monitor, NULL);

    /* Create writer thread. */
	pthread_create(&writer_thread, NULL, writer, NULL);

    /* Join on completion of reader thread update completion status. */
	pthread_join(reader_thread, NULL);
    task_set_complete(reader_task);

    /* Join on completion of producer threads update completion status. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
    task_set_complete(producer_task);
	producer_end = time(NULL);

//    printf("Producer Time: %ld:%02ld.\n",
//           (producer_end - producer_start) / 60, (producer_end - producer_start) % 60);

    pthread_join(monitor_thread, NULL);
    task_set_complete(consumer_task);

    /* Join on completion of writer threads. */
	pthread_join(writer_thread, NULL);
	end = time(NULL);

	/* Heap maintenance. */
    destroy_queues();

	/* Report timing. */
	fprintf(stderr,
			"\nTotal Time: %ld:%02ld\nProducer Time: %ld:%02ld\nConsumer Time: %ld:%02ld\n",
			(end - start) / 60, (end - start) % 60,
			(producer_end - producer_start) / 60, (producer_end - producer_start) % 60,
			(consumer_end - consumer_start) / 60, (consumer_end - consumer_start) % 60);
	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX, sizeof(transform_t *));
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX, sizeof(transform_t *));
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE, sizeof(transform_t *));
    run_queue       = atomic_queue_create(CONSUMER_THREAD_MAX, sizeof(thread_t *));
    reader_task     = task_create();
    producer_task     = task_create();
    consumer_task     = task_create();
}

void destroy_queues() {
    atomic_queue_destroy(input_queue);
    atomic_queue_destroy(output_queue);
    atomic_queue_destroy(work_queue);
    atomic_queue_destroy(run_queue);
    task_destroy(reader_task);
    task_destroy(producer_task);
    task_destroy(consumer_task);
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
            transform_set_cmd(t, cmd);
            transform_set_key(t, key);
            transform_set_seq(t, ++writer_pos);
            atomic_queue_push(input_queue, t);
        }
    }
    pthread_exit(arg);
}

void *producer(void *arg) {
    transform_t *task;
    uint16_t encoded_key;
    double retval;
    // Exits on completion of reader once the input_queue is empty.
    while(!task_is_complete(reader_task) || atomic_queue_size(input_queue)) {
        // Will automatically terminate on the end of all reader input
        if(!(task = (transform_t *) try_queue_pop(input_queue)))
            continue;
        encoded_key = 0;
        switch(transform_get_cmd(task)) {
            case 'A':
                encoded_key = transformA1(transform_get_key(task), &retval);
                break;
            case 'B':
                encoded_key = transformB1(transform_get_key(task), &retval);
                break;
            case 'C':
                encoded_key = transformC1(transform_get_key(task), &retval);
                break;
            case 'D':
                encoded_key = transformD1(transform_get_key(task), &retval);
                break;
            case 'E':
                encoded_key = transformE1(transform_get_key(task), &retval);
                break;
            default:
                break;
        }
        if(encoded_key) {
            transform_set_ekey(task, encoded_key);
            transform_set_eret(task, retval);
            transform_set_pos(task, atomic_queue_pos(work_queue));
            atomic_queue_push(work_queue, task);
        }
    }
    pthread_exit(arg);
}

void *monitor(void *arg) {
    uint16_t state = EMPTY_BUFFER;
    consumer_start = time(NULL);
    while(!task_is_complete(producer_task))
        monitor_work((transform_t *) atomic_queue_top(work_queue), &state);
    complete_consumption();
    pthread_exit(arg);
}

void monitor_work(transform_t *t, uint16_t *state) {
    int rq;
    thread_t *thread;
    if(!t)
        return;
    else if(get_state(atomic_queue_size(work_queue), state))
        report_state_change(t, *state);
    rq = atomic_queue_size(run_queue);
    switch(*state) {
        case BELOW_LOWER:
            // Kill threads
            if(rq && (thread = (thread_t *) try_queue_pop(run_queue)))
                thread_stop(thread);
            break;
        case ABOVE_LOWER:
        case ABOVE_UPPER:
        case FULL_BUFFER:
            // Only triggers once for above lower
            if(((!rq) || (*state != ABOVE_LOWER && rq < CONSUMER_THREAD_MAX - 1)) &&
               ((thread = thread_create(consumer)) != NULL))
                atomic_queue_push(run_queue, thread);
            break;
        default:
            break;
    }
}

void complete_consumption() {
    thread_t *t;
    while(atomic_queue_size(run_queue) < CONSUMER_THREAD_MAX - 1)
        if((t = thread_create(consumer))) atomic_queue_push(run_queue, t);
    while(atomic_queue_size(work_queue) || atomic_queue_size(output_queue))
        ;
    while(atomic_queue_size(run_queue))
        if((t = (thread_t *) try_queue_pop(run_queue))) thread_stop(t);
    consumer_end = time(NULL);
}

void *consumer(void *arg) {
    transform_t *task;
    thread_t *self;
    uint16_t decoded_key;
    double ret;
    // Get reference to thread struct.
    self = (thread_t *) arg;
    while(thread_running(self)) {
        if(!(task = (transform_t *) try_queue_pop(work_queue)))
            continue;
        decoded_key = 0;
        switch(transform_get_cmd(task)) {
            case 'A':
                decoded_key = transformA2(transform_get_ekey(task), &ret);
                break;
            case 'B':
                decoded_key = transformB2(transform_get_ekey(task), &ret);
                break;
            case 'C':
                decoded_key = transformC2(transform_get_ekey(task), &ret);
                break;
            case 'D':
                decoded_key = transformD2(transform_get_ekey(task), &ret);
                break;
            case 'E':
                decoded_key = transformE2(transform_get_ekey(task), &ret);
                break;
            default:
                break;
        }
        if(decoded_key) {
            transform_set_dkey(task, decoded_key);
            transform_set_dret(task, ret);
            atomic_queue_push(output_queue, task);
        }
    }
    // Wait to ensure monitor has a chance to join
    sleep(2 * SLEEP_INTERVAL);
    pthread_exit(NULL);
}

void *writer(void *arg) {
    heap_t *h;
    transform_t *t;
    int32_t exp_seq = 1;
    h = create_heap();
    // Wait for reader to process first item
    while(!task_is_complete(consumer_task) || atomic_queue_size(output_queue) > 0 || heap_size(h) > 0) {
        if((t = (transform_t *) try_queue_pop(output_queue)))
            insert(h, t, transform_get_seq(t));
        else if(minimum(h) == exp_seq) {
            t = (transform_t *) extract_min(h);
            fprintf(stdout, "%6d %6d %6c %6hu %23.1lf %6hu %23.1lf\n",
                    transform_get_seq(t), transform_get_pos(t), transform_get_cmd(t),
                    transform_get_ekey(t), transform_get_eret(t), transform_get_dkey(t),
                    transform_get_dret(t));
            transform_destroy(t);
            exp_seq++;
        } else
            sleep(SLEEP_INTERVAL);
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
bool get_state(int size, uint16_t *state) {
    uint16_t x = 0x0;
    x       = (!size)                    ? x | 0x8 : x & ~(0x1 << 0x3);
    x       = (size >  WORK_MIN_THRESH)  ? x | 0x4 : x & ~(0x1 << 0x2);
    x       = (size >  WORK_MAX_THRESH)  ? x | 0x2 : x & ~(0x1 << 0x1);
    x       = (size == WORK_BUFFER_SIZE) ? x | 0x1 : x & ~(0x1 << 0x0);
    if(x == *state) return false;
    *state = x;
    return true;
}

void report_state_change(transform_t *t, uint16_t state) {
    switch(state) {
        case EMPTY_BUFFER:
            fprintf(stderr, "Work Queue Empty.\n");
            break;
        case BELOW_LOWER:
            fprintf(stderr,
                    "\nWork Queue Below Lower: Item = %d  Cmd = %c   Key = %hu.\n",
                    transform_get_seq(t), transform_get_cmd(t), transform_get_key(t));
            break;
        case ABOVE_LOWER:
            fprintf(stderr,
                    "\nWork Queue Above Lower: Item = %d  Cmd = %c   Key = %hu.\n",
                    transform_get_seq(t), transform_get_cmd(t), transform_get_key(t));
            break;
        case ABOVE_UPPER:
            fprintf(stderr,
                    "\nWork Queue Above Upper: Item = %d  Cmd = %c   Key = %hu.\n",
                    transform_get_seq(t), transform_get_cmd(t), transform_get_key(t));
            break;
        case FULL_BUFFER:
            fprintf(stderr,
                    "\nWork Queue Full: Item = %d  Cmd = %c   Key = %hu.\n",
                    transform_get_seq(t), transform_get_cmd(t), transform_get_key(t));
            break;
        default:
            break;
    }
}






