/*
 * Main handler for the four major thread pool types. Instantiate threads,
 * extern buffer types, and buffer signals to signify completion of each
 * thread group.
 */

#include <stdio.h>
#include <stdbool.h>

#include "atomic_queue.h"
#include "threads.h"

#define PRODUCER_THREADS        4
#define CONSUMER_THREAD_MAX     20
#define INPUT_BUFFER_COUNT      3
#define INPUT_BUFFER_MIN        50
#define INPUT_BUFFER_MAX        200
#define WORK_BUFFER_SIZE        200
#define WORK_MAX_THRESH         150
#define WORK_MIN_THRESH         50
#define OUTPUT_BUFFER_COUNT     3
#define OUTPUT_BUFFER_MIN       50
#define OUTPUT_BUFFER_MAX       200

/*
 * Creates the three major blocking queue types.
 */
void make_queues();

/*
 * Declares signaling extern booleans and position of the current
 * writer operation.
 */
void set_booleans();

/*
 * Entry function for main program.
 */
int main() {
    // Declare counter and thread pools.
	uint16_t i;
	pthread_t   reader_thread,
				writer_thread,
				consumer_thread[CONSUMER_THREAD_MAX],
				producer_threads[PRODUCER_THREADS];
    // Instantiate queues and other variables.
	make_queues();
	set_booleans();

    // Create reader thread.
	pthread_create(&reader_thread, NULL, writer, NULL);

    // Create producer threads.
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_create(&producer_threads[i], NULL, producer, NULL);

    // Create consumer threads.
	for(i = 0; i < CONSUMER_THREAD_MAX; ++i)
		pthread_create(&consumer_thread[i], NULL, consumer, NULL);

    // Create reader thread.
	pthread_create(&writer_thread, NULL, writer, NULL);

    // Join on completion of reader thread.
	pthread_join(reader_thread, NULL);
	// TODO: Add locking mechanism to reader_done.
    reader_done = true;

    // Join on completion of producer threads.
	for(i = 0; i < PRODUCER_THREADS; ++i)
		pthread_join(producer_threads[i], NULL);
    // TODO: Add locking mechanism to producer_done.
	producer_done = true;

    // Join on completion of consumer threads.
    // TODO: Dynamically scale consumer thread pool.
	for(i = 0; i < CONSUMER_THREAD_MAX; ++i)
		pthread_join(consumer_thread[i], NULL);
    // TODO: Add locking mechanism for consumer_done.
	consumer_done = true;

    // Join on completion of writer threads.
	pthread_join(writer_thread, NULL);

	return 0;
}

void make_queues() {
    input_queue     = atomic_queue_create(INPUT_BUFFER_MAX, 2);
    work_queue      = atomic_queue_create(WORK_BUFFER_SIZE, 2);
    output_queue    = atomic_queue_create(OUTPUT_BUFFER_MAX, 2);
}

void set_booleans() {
    writer_pos      = 0;
    reader_done     = false;
    producer_done   = false;
    consumer_done   = false;
}
