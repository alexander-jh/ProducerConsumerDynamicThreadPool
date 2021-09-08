#include "threads.h"

void *reader(void *arg) {
	char cmd;
	uint16_t key;
	transform_t *t = task_create();
    /* Using STDIN for file input */
    while(fscanf(stdin, "%c %hu", &cmd, &key)) {
		if(cmd == 'X') break;
		set_cmd(t, cmd);
		set_key(t, key);
		set_seq_num(t, ++writer_pos);
		atomic_queue_add(input_queue, t);
		t = task_create();
	}
	pthread_exit(arg);
}

void *producer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!producer_done) {
		task = (transform_t *) atomic_queue_remove(input_queue, false);

        if(task == NULL && reader_done)
            break;
        else if(task == NULL)
            continue;

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
	pthread_exit(arg);
}

void *consumer(void *arg) {
	transform_t *task;
	uint16_t out_val;
	double retval;
	while(!consumer_done) {
		task = (transform_t *) atomic_queue_remove(work_queue, true);

        if(task == NULL && producer_done)
            break;
        else if(task == NULL)
            continue;

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
	pthread_exit(arg);
}

void *writer(void *arg) {
	transform_t *t;
    while(true) {
        t = (transform_t *) atomic_queue_remove(output_queue, false);
        if(t == NULL && consumer_done)
            pthread_exit(arg);
        else if(t != NULL)
            printf("%hu", get_decoded_key(t));
    }
}
