/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include "atomic_queue.h"
#include "threads.h"

atomic_queue_t  *input_queue,
				*work_queue,
				*output_queue,
				*run_queue;

atomic_int      reader_done,
				producer_done,
				consumer_done,
				alert_waiting,
				total_produced,
				total_consumed;

sem_t           lower_thresh,
				upper_thresh,
				full_buffer,
				empty_buffer;

/*
 * Entry function for main program.
 */
int main(void) {
    /* Declare counter, timers and thread pools. */
	int i;
	time_t      start,
				producer_start,
				consumer_start,
				end,
				producer_end,
				consumer_end;
	pthread_t   reader_thread,
				writer_thread,
				monitor_thread,
				producer_threads[PRODUCER_THREADS];
	/* Instantiate queues and other variables. */
	make_queues();
	make_semaphores();
	set_booleans();

	/* Mark the start of the program */
	start = time(NULL);

    /* Create reader thread. */
	pthread_create(&reader_thread, NULL, reader, NULL);

    /* Create producer threads. */
	producer_start = time(NULL);
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    /* Create monitor thread. */
	consumer_start = time(NULL);
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
	producer_done = 1;

    /* Join on completion of consumer/monitor threads update completion staus. */
    pthread_join(monitor_thread, NULL);
	consumer_end = time(NULL);
	consumer_done = 1;

    /* Join on completion of writer threads. */
	pthread_join(writer_thread, NULL);
	end = time(NULL);

	/* Heap maintenance. */
    destroy_queues();
	destroy_semaphores();

	/* Report timing. */
	fprintf(stderr,
			"Total Time: %ld:%02ld\nProducer Time: %ld:%02ld\nConsumer Time: %ld:%02ld\n",
			(end - start) / 60, (end - start) % 60,
			(producer_end - producer_start) / 60, (producer_end - producer_start) % 60,
			(consumer_end - consumer_start) / 60, (consumer_end - consumer_start) % 60);

	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX, sizeof(transform_t *));
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE, sizeof(transform_t *));
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX, sizeof(transform_t *));
    run_queue       = atomic_queue_create(CONSUMER_THREAD_MAX, sizeof(thread_t *));
}

void make_semaphores() {
	sem_init(&lower_thresh, 0, 0);
	sem_init(&upper_thresh, 0, 0);
	sem_init(&full_buffer, 0, 0);
	sem_init(&empty_buffer, 0, 0);
}

void destroy_queues() {
    atomic_queue_destroy(input_queue);
    atomic_queue_destroy(work_queue);
    atomic_queue_destroy(output_queue);
    atomic_queue_destroy(run_queue);
}

void destroy_semaphores(){
	sem_close(&lower_thresh);
	sem_close(&upper_thresh);
	sem_close(&full_buffer);
	sem_close(&empty_buffer);
}

void set_booleans() {
	reader_done         = 0;
	producer_done       = 0;
	consumer_done       = 0;
	alert_waiting       = 0;
	total_produced      = 0;
	total_consumed      = 0;
}