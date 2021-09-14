/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include "atomic_queue.h"
#include "threads.h"

atomic_queue_t  *input_queue;
atomic_queue_t	*work_queue;
atomic_queue_t	*output_queue;

atomic_int      reader_done;
atomic_int  	producer_done;
atomic_int		consumer_done;

atomic_int      writer_pos;

/*
 * Entry function for main program.
 */
int main(void) {
    /* Declare counter and thread pools. */
	uint16_t i;
	pthread_t   reader_thread,
				writer_thread,
				monitor_thread,
                //consumer_threads[CONSUMER_THREAD_MAX],
				producer_threads[PRODUCER_THREADS];
	/* Instantiate queues and other variables. */
	make_queues();
	set_booleans();

    /* Create reader thread. */
	pthread_create(&reader_thread, NULL, reader, NULL);

    /* Create producer threads. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    /* Create monitor threads. */
    pthread_create(&monitor_thread, NULL, monitor, NULL);

    /* Create writer thread. */
	pthread_create(&writer_thread, NULL, writer, NULL);

    /* Join on completion of reader thread. */
	pthread_join(reader_thread, NULL);
    reader_done = 1;
    printf("\nReader done\n");

    /* Join on completion of producer threads. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
	producer_done = 1;
    printf("\nProducers done\n");

    /* Join on completion of consumer monitor thread. */
    pthread_join(monitor_thread, NULL);
	consumer_done = 1;
    printf("\nConsumers done\n");

    /* Join on completion of writer threads. */
	pthread_join(writer_thread, NULL);
    printf("\nWriter done\n");

    destroy_queues();

	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX);
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE);
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX);
}

void destroy_queues() {
    atomic_queue_destroy(input_queue);
    atomic_queue_destroy(work_queue);
    atomic_queue_destroy(output_queue);
}

void set_booleans() {
    writer_pos          = 0;
	reader_done         = 0;
	producer_done       = 0;
	consumer_done       = 0;
}
