/*
 * Data structure for holding values in through the transformation
 * process.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

/*
 * Struct to contain all the required values throughout the process
 * (char) cmd, (uint16_t) key, (int) sequence_number,
 * (int) work_queue_position, (uint16_t) encoded and decoded key,
 * (double) encoded and decoded return from transforms.
 */
typedef struct transform_struct transform_t;

// Struct for containing the consumer threads. This is necessary
// to have a shared memory space so the monitor thread can softly
// shut down threads and ensure the operation completes.
typedef struct thread_struct thread_t;

// No-arg constructor for transform object.
transform_t *transform_create(void);

// Destructor for the transform object.
void task_destroy(transform_t *t);

/*
 * Since transform_t is referenced as a void pointer, direct access to
 * the struct is not possible. So default getter/setter functions
 * were created for each field.
 */

void set_cmd(transform_t *t, char cmd);

void set_key(transform_t *t, uint16_t key);

void set_seq_num(transform_t *t, int seq_num);

void set_queue_pos(transform_t *t, int queue_pos);

void set_encoded_key(transform_t *t, uint16_t encoded_key);

void set_decoded_key(transform_t *t, uint16_t decoded_key);

void set_encoded_ret(transform_t *t, double encoded_ret);

void set_decoded_ret(transform_t *t, double decoded_ret);

char get_cmd(transform_t *t);

uint16_t get_key(transform_t *t);

int get_seq_num(transform_t *t);

int get_queue_pos(transform_t *t);

uint16_t get_encoded_key(transform_t *t);

uint16_t get_decoded_key(transform_t *t);

double get_encoded_ret(transform_t *t);

double get_decoded_ret(transform_t *t);
