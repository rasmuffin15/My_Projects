#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include "multi-lookup.h"
#include "util.h"

bool finish_read = false;
bool is_empty = true;
bool is_full = false;

char *req_file, *res_file;
queue job_q, shared_q;

pthread_mutex_t rem_lock;
pthread_mutex_t add_lock;
pthread_mutex_t req_write_lock;
pthread_mutex_t res_write_block;

int queue_init(int q_size, queue *q) {
	q->q_curr_size = 0;
	q->q_fixed_size = q_size;
	q->first = 0;
	q->last = 0;
	q->q_array = malloc(q->q_fixed_size * sizeof(char *));
	return q->q_fixed_size;
}

int queue_DESTROY(queue *q) {
	free(q->q_array);
	return 0;
}

int queue_isfull(queue *q) {

	return(!(q->q_fixed_size - q->q_curr_size));
}

int queue_isempty(queue *q) {

	return (!q->q_curr_size);
}

int queue_add(queue *q, char *file) {
	
	pthread_mutex_lock(&add_lock);
	
	if(queue_isfull(q)) {
		pthread_mutex_unlock(&add_lock);
		return 0;
	}
	
	q->q_array[q->last] = strdup(file);
	q->last++;
	q->q_curr_size++;

	if(q->last == q->q_fixed_size)
		q->last = 0;

	pthread_mutex_unlock(&add_lock);
	return q->q_curr_size;
}

char *queue_rem(queue *q) {

	pthread_mutex_lock(&rem_lock);
	if(queue_isempty(q)) {
		pthread_mutex_unlock(&rem_lock);
		return NULL;
	}

	char *temp = q->q_array[q->first];
	q->q_array[q->first] = NULL;
	q->first++;
	q->q_curr_size--;

	if(q->first == q->q_fixed_size)
		q->first = 0;
	pthread_mutex_unlock(&rem_lock);
	return temp;
}

void *request_func() {

	char hostname[1024];
	char * f;
	int num_file_serviced = 0;
	
	while ((f = queue_rem(&job_q)) != NULL){

		FILE *fp;
		if ((fp = fopen(f, "r")) == NULL) {
			fprintf(stderr, "Cannot open file '%s'\n", f);
			continue;
		}
		
		while(fscanf(fp, "%1024s", hostname) != EOF) {
			//printf("Adding hostname %s serviced by %lu\n", hostname, pthread_self());
			while(!queue_add(&(shared_q), hostname));
	
		}

		fclose(fp);
		num_file_serviced++;
		//printf("Closed File\n");
	}

	pthread_mutex_lock(&req_write_lock);
	FILE *serv = fopen(req_file, "a+");

	if(serv == NULL) {
		perror("Could not open output file");
		exit(-1);
	}

	fprintf(serv, "Pthread %lu serviced %i files\n", pthread_self(), num_file_serviced);
	fclose(serv);
	pthread_mutex_unlock(&req_write_lock);
	return 0;
}


void *resolve_func() {
	//printf("Inside Resolve function: %lu\n", pthread_self());

	char *name;
	char firstipstr[16];
	//name = queue_rem(&shared_q);
	//printf("hostname being resolved: %s\n", name);
	
	while ((name = queue_rem(&shared_q)) != NULL || finish_read == false) {
		if(name == NULL)
			continue;
		//printf("hostname being resolved by thread %lu: %s\n", pthread_self(), name);
		
		if(dnslookup(name, firstipstr, sizeof(firstipstr)) == -1) {
			fprintf(stderr, "Cannot lookup '%s'\n", name);
			firstipstr[0] = '\0';
		}
		pthread_mutex_lock(&res_write_block);
		FILE *ip = fopen(res_file, "a+");

		if(ip == NULL) {
			perror("Could not open output file");
			exit(-1);
		}

		fprintf(ip, "%s,%s\n", name, firstipstr);
		fclose(ip);
		pthread_mutex_unlock(&res_write_block);
		//printf("apples: %s\n", firstipstr);
	}
	return 0;
	//printf("Exiting Resolve Func %lu\n", pthread_self());
}

int main(int argc, char *argv[]) {

	struct timeval start, end;
	double total;
	gettimeofday(&start, 0);

	int num_req_threads = atoi(argv[1]);
	int num_res_threads = atoi(argv[2]);
	req_file = argv[3];
	res_file = argv[4];
	//int num_res_threads = atoi(argv[2]);
	pthread_t req_thread[num_req_threads];
	pthread_t res_thread[num_res_threads];
	int n_files = argc - 5;

	queue_init(n_files, &(job_q));
	queue_init(10, &(shared_q));
	
	//printf("queue size is: %i\n", kk);
	// Add all the filenames to the queue.
	for (int i = 0; i < n_files; i++) {
		//printf("Adding file %s to q\n", argv[(argc - n_files) + i]);
		queue_add(&job_q, argv[(argc - n_files) + i]);
		//printf("File Added: %s\n", job_q.q_array[(argc - n_files) + i]);
	}
	
	for(int i = 0; i < num_req_threads; i++)
		pthread_create(&req_thread[i], NULL, request_func, NULL);

	for(int k = 0; k < num_res_threads; k++)
		pthread_create(&res_thread[k], NULL, resolve_func, NULL);

	for(int j = 0; j < num_req_threads; j++) 
		pthread_join(req_thread[j], NULL);

	finish_read = true;

	for(int l = 0; l < num_res_threads; l++)
		pthread_join(res_thread[l], NULL);

	queue_DESTROY(&job_q);
	queue_DESTROY(&shared_q);
	gettimeofday(&end, 0);
	total = (end.tv_sec - start.tv_sec);

	printf("Total Runtime is: %f\n", total);
	return 0;
}