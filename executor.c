/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include "atomic_queue.h"
#include "threads.h"

flag_t           *flags;

atomic_queue_t  *input_queue;
atomic_queue_t	*work_queue;
atomic_queue_t	*output_queue;
atomic_queue_t  *run_queue;

atomic_int      reader_done;
atomic_int  	producer_done;
atomic_int		consumer_done;

atomic_int      writer_pos;

/*
 * Entry function for main program.
 */
int main(void) {
    /* Declare counter and thread pools. */
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
	set_booleans();

	/* Mark the start of the program */
	start = time(NULL);

    /* Create reader thread. */
	pthread_create(&reader_thread, NULL, reader, NULL);

    /* Create producer threads. */
	producer_start = time(NULL);
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    /* Create monitor threads. */
	consumer_start = time(NULL);
    pthread_create(&monitor_thread, NULL, monitor, NULL);

    /* Create writer thread. */
	pthread_create(&writer_thread, NULL, writer, NULL);

    /* Join on completion of reader thread. */
	pthread_join(reader_thread, NULL);
    reader_done = 1;

    /* Join on completion of producer threads. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
	producer_end = time(NULL);
	producer_done = 1;

    /* Join on completion of consumer monitor thread. */
    pthread_join(monitor_thread, NULL);
	consumer_end = time(NULL);
	consumer_done = 1;

    /* Join on completion of writer threads. */
	pthread_join(writer_thread, NULL);
	end = time(NULL);
    destroy_queues();

	fprintf(stderr,
			"Total Time: %ld:%02ld\nProducer Time: %ld:%02ld\nConsumer Time(seconds): %ld:%02ld\n",
			(end - start) / 60, (end - start) % 60,
			(producer_end - producer_start) / 60, (producer_end - producer_start) % 60,
			(consumer_end - consumer_start) / 60, (consumer_end - consumer_start) % 60);

	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX);
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE);
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX);
    run_queue       = atomic_queue_create(CONSUMER_THREAD_MAX);
	flags           = create_flags();
}

void destroy_queues() {
    atomic_queue_destroy(input_queue);
    atomic_queue_destroy(work_queue);
    atomic_queue_destroy(output_queue);
    atomic_queue_destroy(run_queue);
	flag_delete(flags);
}

void set_booleans() {
    writer_pos          = 0;
	reader_done         = 0;
	producer_done       = 0;
	consumer_done       = 0;
}
