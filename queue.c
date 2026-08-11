#include <stdio.h>

#include "queue.h"
#include "../logger/logger.h"

void initQueue(Queue *q)
{
    if(q == NULL)
    {
        return;
    }

    q->front = -1;
    q->rear = -1;
}

int isEmpty(Queue *q)
{
    if(q == NULL)
    {
        return 1;
    }

    return (q->front == -1);
}

void enqueue(Queue *q, int value)
{
    if(q == NULL)
    {
        return;
    }

    if(q->rear == (MAX_STATIONS - 1))
    {
        logError("Queue Overflow");

        printf("\nQueue Overflow\n");

        return;
    }

    if(q->front == -1)
    {
        q->front = 0;
    }

    q->rear++;

    q->items[q->rear] = value;
}

int dequeue(Queue *q)
{
    if(q == NULL)
    {
        return -1;
    }

    if(isEmpty(q))
    {
        logWarning("Queue Underflow");

        return -1;
    }

    int item = q->items[q->front];

    if(q->front >= q->rear)
    {
        q->front = -1;
        q->rear = -1;
    }
    else
    {
        q->front++;
    }

    return item;
}
