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

bool            reader_done;
bool			producer_done;
bool			consumer_done;

uint16_t        writer_pos;


/*
 * Entry function for main program.
 */
int main(void) {
    /* Declare counter and thread pools. */
	uint16_t i;
	pthread_t   reader_thread,
				writer_thread,
				consumer_thread[CONSUMER_THREAD_MAX],
				producer_threads[PRODUCER_THREADS];
	/* Instantiate queues and other variables. */
	make_queues();
	set_booleans();

    /* Create reader thread. */
	pthread_create(&reader_thread, NULL, reader, NULL);

    /* Create producer threads. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    /* Create consumer threads. */
	for(i = 0; i < CONSUMER_THREAD_MAX; ++i)
		pthread_create(&consumer_thread[i], NULL, consumer, NULL);

    /* Create reader thread. */
	pthread_create(&writer_thread, NULL, writer, NULL);

    /* Join on completion of reader thread. */
	pthread_join(reader_thread, NULL);
	/* TODO: Add locking mechanism to reader_done. */
    reader_done = true;

    /* Join on completion of producer threads. */
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
    /* TODO: Add locking mechanism to producer_done. */
	producer_done = true;

    /* Join on completion of consumer threads. */
    /* TODO: Dynamically scale consumer thread pool. */
	for(i = 0; i < CONSUMER_THREAD_MAX; ++i)
		pthread_join(consumer_thread[i], NULL);
    /* TODO: Add locking mechanism for consumer_done. */
	consumer_done = true;

    /* Join on completion of writer threads. */
	pthread_join(writer_thread, NULL);

	return 0;
}

void make_queues() {
	input_queue     = atomic_queue_create(INPUT_BUFFER_MAX);
	work_queue      = atomic_queue_create(WORK_BUFFER_SIZE);
	output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX);
}

void set_booleans() {
	writer_pos      = 0;
	reader_done     = false;
	producer_done   = false;
	consumer_done   = false;
}