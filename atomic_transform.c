#include "atomic_transform.h"

struct transform_struct {
    char        cmd;
    int         queue_pos;
    double      encoded_ret,
                decoded_ret;
    uint16_t    key,
                seq_num,
                encoded_key,
                decoded_key;
};

transform_t *transform_create() {
    transform_t *t = (transform_t *) malloc(sizeof(transform_t)) MPANIC(t);
    return t;
}

void transform_destroy(transform_t *t) {
    free(t);
}

void transform_set_cmd(transform_t *t, char c) {
    t->cmd = c;
}

void transform_set_pos(transform_t *t, int i) {
    t->queue_pos = i;
}

void transform_set_eret(transform_t *t, double d) {
    t->encoded_ret = d;
}

void transform_set_dret(transform_t *t, double d) {
    t->decoded_ret = d;
}

void transform_set_key(transform_t *t, uint16_t u) {
    t->key = u;
}

void transform_set_seq(transform_t *t, uint16_t u) {
    t->seq_num = u;
}

void transform_set_ekey(transform_t *t, uint16_t u) {
    t->encoded_key = u;
}

void transform_set_dkey(transform_t *t, uint16_t u) {
    t->decoded_key = u;
}

char transform_get_cmd(transform_t *t) {
    return t->cmd;
}

int transform_get_pos(transform_t *t) {
    return t->queue_pos;
}

double transform_get_eret(transform_t *t) {
    return t->encoded_ret;
}

double transform_get_dret(transform_t *t) {
    return t->decoded_ret;
}

uint16_t transform_get_key(transform_t *t) {
    return t->key;
}

uint16_t transform_get_seq(transform_t *t) {
    return t->seq_num;
}

uint16_t transform_get_ekey(transform_t *t) {
    return t->encoded_key;
}

uint16_t transform_get_dkey(transform_t *t) {
    return t->decoded_key;
}


