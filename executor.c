/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include "threads.h"

atomic_queue_t          *input_queue,
                        *work_queue,
                        *run_queue,
                        *output_queue;
atomic_int              produced,
                        written,
                        reader_done;
_Atomic(uint16_t)       state;
_Atomic(void *)         *current_task;
time_t                  start,
                        producer_start,
                        consumer_start,
                        end,
                        producer_end,
                        consumer_end;

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
    reader_done = 1;

    /* Join on completion of producer threads update completion status. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
	producer_end = time(NULL);

//    printf("Producer Time: %ld:%02ld.\n",
//           (producer_end - producer_start) / 60, (producer_end - producer_start) % 60);

    pthread_join(monitor_thread, NULL);

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

    /* Let remaining consumer threads have some time to cancel */
    sleep(SLEEP_INTERVAL * 10);
	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX, sizeof(transform_t *));
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX, sizeof(transform_t *));
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE, sizeof(transform_t *));
    run_queue       = atomic_queue_create(BURN_THRESHOLD, sizeof(thread_t *));
    produced        = 0;
    written         = 0;
    state           = EMPTY_BUFFER;
    current_task    = NULL;
}

void destroy_queues() {
    atomic_queue_destroy(input_queue);
    atomic_queue_destroy(output_queue);
    atomic_queue_destroy(work_queue);
    atomic_queue_destroy(run_queue);
}
