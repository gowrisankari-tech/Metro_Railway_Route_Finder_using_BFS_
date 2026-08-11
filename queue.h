#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#include "../graph/graph.h"

typedef struct
{
    int items[MAX_STATIONS];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, int value);
int dequeue(Queue *q);

#endif
