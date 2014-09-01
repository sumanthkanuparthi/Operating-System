#ifndef QUEUE_H
#define QUEUE_H


#include <sys/contextswitch.h>

#define QUEUESIZE       15

typedef struct {
        task_struct* q[QUEUESIZE+1];		/* body of queue */
        int first;                      /* position of first element */
        int last;                       /* position of last element */
        int count;                      /* number of queue elements */
} queue;

task_struct * dequeue(queue *q);
int empty(queue *q);

void enqueue(queue *q, task_struct *x);

void init_queue(queue *q);

#endif
