#include "threads.h"

void *reader(void *arg) {
	FILE *fp;
	char cmd;
	uint16_t key;
	fp = fopen((const char *) arg, "r");
	if(!fp) {
		perror("Error: Failed to open input file.\n");
		exit(EXIT_FAILURE);
	}
	transform_t *t = task_create();
	while(fscanf(fp, "%c %hu", &cmd, &key)) {
		set_cmd(t, cmd);
		set_key(t, key);
		set_seq_num(t, ++writer_pos);
		atomic_queue_add(input_queue, t);
		t = task_create();
	}
	pthread_exit(0);
}

void *producer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!producer_done) {
		task = (transform_t *) _atomic_queue_try_remove(input_queue, false);
		if(task == NULL && reader_done) break;
		switch(get_cmd(task)) {
			case 'A':
				out_val = transformA1(get_key(task), &retval);
				break;
			case 'B':
				out_val = transformB1(get_key(task), &retval);
				break;
			case 'C':
				out_val = transformC1(get_key(task), &retval);
				break;
			case 'D':
				out_val = transformD1(get_key(task), &retval);
				break;
			case 'E':
				out_val = transformE1(get_key(task), &retval);
				break;
			default:
				continue;
		}
		set_encoded_key(task, out_val);
		set_encoded_ret(task, retval);
		atomic_queue_add(work_queue, task);
	}
	pthread_exit(0);
}

void *consumer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!consumer_done) {
		task = (transform_t *) _atomic_queue_try_remove(work_queue, true);
		if(task == NULL && producer_done) break;
		switch(get_cmd(task)) {
			case 'A':
				out_val = transformA2(get_encoded_key(task), &retval);
				break;
			case 'B':
				out_val = transformB2(get_encoded_key(task), &retval);
				break;
			case 'C':
				out_val = transformC2(get_encoded_key(task), &retval);
				break;
			case 'D':
				out_val = transformD2(get_encoded_key(task), &retval);
				break;
			case 'E':
				out_val = transformE2(get_encoded_key(task), &retval);
				break;
			default:
				continue;
		}
		set_decoded_key(task, out_val);
		set_decoded_ret(task, retval);
		atomic_queue_add(output_queue, task);
	}
	pthread_exit(0);
}

void *writer(void *arg) {
	transform_t *t;
	while((t = (transform_t *) atomic_queue_remove(output_queue, false))) {
		printf("%hu\n", get_decoded_key(t));
		task_destroy(t);
	}
	pthread_exit(0);
}