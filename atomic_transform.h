#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MPANIC(x) ;     assert(x != NULL)

/**
 * Struct to contain all the required values throughout the process
 * (char) cmd, (uint16_t) key, (int) sequence_number,
 * (int) work_queue_position, (uint16_t) encoded and decoded key,
 * (double) encoded and decoded return from transforms.
 */
typedef struct transform_struct transform_t;

/**
 * No-arg constructor for transform object.
 */
transform_t *transform_create(void);

/**
 * Destructor for the transform struct.
 * @param   - reference to a transform struct to free
 */
void transform_destroy(transform_t *);

/**
 * Standard getter/setter functions for each element in the struct
 * to ensure type safe access.
 */

void transform_set_cmd(transform_t *, char);

void transform_set_pos(transform_t *, int);

void transform_set_eret(transform_t *, double);

void transform_set_dret(transform_t *, double);

void transform_set_key(transform_t *, uint16_t);

void transform_set_seq(transform_t *, uint16_t);

void transform_set_ekey(transform_t *, uint16_t);

void transform_set_dkey(transform_t *, uint16_t);

char transform_get_cmd(transform_t *);

int transform_get_pos(transform_t *);

double transform_get_eret(transform_t *);

double transform_get_dret(transform_t *);

uint16_t transform_get_key(transform_t *);

uint16_t transform_get_seq(transform_t *);

uint16_t transform_get_ekey(transform_t *);

uint16_t transform_get_dkey(transform_t *);
