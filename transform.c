#include "transform.h"

struct transform_struct {
	char        cmd;
	uint16_t    key;
	uint16_t    seq_num;
	int         queue_pos;
	uint16_t	encoded_key;
	uint16_t    decoded_key;
	double      encoded_ret;
	double      decoded_ret;
};

transform_t *transform_create() {
	transform_t *t = calloc(1, sizeof(transform_t));
	if(!t) {
		perror("Error: Failed to allocate memory for task_t.\n");
		exit(EXIT_FAILURE);
	}
	return t;
}

void task_destroy(transform_t *t) {
	free(t);
}

void set_cmd(transform_t *t, char cmd) { t->cmd = cmd; }

void set_key(transform_t *t, uint16_t key) { t->key = key; }

void set_seq_num(transform_t *t, uint16_t seq_num) { t->seq_num = seq_num; }

void set_queue_pos(transform_t *t, int queue_pos) { t->queue_pos = queue_pos; }

void set_encoded_key(transform_t *t, uint16_t encoded_key) { t->encoded_key = encoded_key; }

void set_decoded_key(transform_t *t, uint16_t decoded_key) { t->decoded_key = decoded_key; }

void set_encoded_ret(transform_t *t, double encoded_ret) { t->encoded_ret = encoded_ret; }

void set_decoded_ret(transform_t *t, double decoded_ret) { t->decoded_ret = decoded_ret; }

char get_cmd(transform_t *t) { return t->cmd; }

uint16_t get_key(transform_t *t) { return t->key; }

uint16_t get_seq_num(transform_t *t) { return t->seq_num; }

int get_queue_pos(transform_t *t) { return t->queue_pos; }

uint16_t get_encoded_key(transform_t *t) { return t->encoded_key; }

uint16_t get_decoded_key(transform_t *t) { return t->decoded_key; }

double get_encoded_ret(transform_t *t) { return t->encoded_ret; }

double get_decoded_ret(transform_t *t) { return t->decoded_ret; }