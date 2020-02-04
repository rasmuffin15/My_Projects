#ifndef MULTILOOKUP
#define MULTILOOKUP

#include <pthread.h>
//structure to store shared data for request thread

typedef struct queue_j {
	char **q_array;
	int q_curr_size, q_fixed_size, first, last;
}queue;

int queue_init(int, queue *);
int queue_add(queue *, char *);
char *queue_rem(queue *);
int queue_isfull(queue *);
int queue_isempty(queue *);
int queue_DESTROY(queue *);

void *request_func();
void *resolve_func();

#endif