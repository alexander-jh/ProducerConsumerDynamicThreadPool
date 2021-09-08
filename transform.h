#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct transform_struct transform_t;

transform_t *task_create(void);

void task_destroy(transform_t *t);

void set_cmd(transform_t *t, char cmd);

void set_key(transform_t *t, uint16_t key);

void set_seq_num(transform_t *t, uint16_t seq_num);

void set_queue_pos(transform_t *t, uint16_t queue_pos);

void set_encoded_key(transform_t *t, uint16_t encoded_key);

void set_decoded_key(transform_t *t, uint16_t decoded_key);

void set_encoded_ret(transform_t *t, double encoded_ret);

void set_decoded_ret(transform_t *t, double decoded_ret);

char get_cmd(transform_t *t);

uint16_t get_key(transform_t *t);

uint16_t get_seq_num(transform_t *t);

uint16_t get_queue_pos(transform_t *t);

uint16_t get_encoded_key(transform_t *t);

uint16_t get_decoded_key(transform_t *t);

double get_encoded_ret(transform_t *t);

double get_decoded_ret(transform_t *t);
