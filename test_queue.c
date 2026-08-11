/*
 * test_queue.c - CUnit tests for queue.c
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stddef.h>

#include "../queue/queue.h"

static void test_initQueue_isEmpty(void)
{
    Queue q;
    initQueue(&q);
    CU_ASSERT_TRUE(isEmpty(&q));
}

static void test_enqueue_dequeue_fifoOrder(void)
{
    Queue q;
    initQueue(&q);

    enqueue(&q, 10);
    enqueue(&q, 20);
    enqueue(&q, 30);

    CU_ASSERT_FALSE(isEmpty(&q));
    CU_ASSERT_EQUAL(dequeue(&q), 10);
    CU_ASSERT_EQUAL(dequeue(&q), 20);
    CU_ASSERT_EQUAL(dequeue(&q), 30);
    CU_ASSERT_TRUE(isEmpty(&q));
}

static void test_dequeue_onEmptyReturnsMinusOne(void)
{
    Queue q;
    initQueue(&q);
    CU_ASSERT_EQUAL(dequeue(&q), -1);
}

static void test_enqueue_dequeue_nullSafe(void)
{
    /* should not crash */
    enqueue(NULL, 5);
    CU_ASSERT_EQUAL(dequeue(NULL), -1);
    CU_ASSERT_EQUAL(isEmpty(NULL), 1);
}

int addQueueTests(void)
{
    CU_pSuite suite = CU_add_suite("queue_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "new queue is empty", test_initQueue_isEmpty) == NULL) ||
       (CU_add_test(suite, "enqueue/dequeue preserves FIFO order", test_enqueue_dequeue_fifoOrder) == NULL) ||
       (CU_add_test(suite, "dequeue on empty queue returns -1", test_dequeue_onEmptyReturnsMinusOne) == NULL) ||
       (CU_add_test(suite, "NULL queue pointer does not crash", test_enqueue_dequeue_nullSafe) == NULL))
    {
        return -1;
    }

    return 0;
}
