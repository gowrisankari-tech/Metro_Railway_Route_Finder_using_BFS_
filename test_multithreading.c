/*
 * test_multithreading.c - CUnit tests for the isolated
 * multithreading module.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdint.h>

#include "../multithreading/multithreading.h"

static void test_workers_produceExpectedCount(void)
{
    uint64_t result = 0U;

    CU_ASSERT_EQUAL(mtRunWorkers(4U, 10000U, &result), 1);
    CU_ASSERT_EQUAL(result, 40000U);
}

static void test_invalidArgumentsRejected(void)
{
    uint64_t result = 123U;

    CU_ASSERT_EQUAL(mtRunWorkers(0U, 10U, &result), 0);
    CU_ASSERT_EQUAL(mtRunWorkers(2U, 0U, &result), 0);
    CU_ASSERT_EQUAL(mtRunWorkers(2U, 10U, NULL), 0);
}

int addMultithreadingTests(void)
{
    CU_pSuite suite = CU_add_suite("multithreading_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "mutex-protected workers produce expected count",
                    test_workers_produceExpectedCount) == NULL) ||
       (CU_add_test(suite, "invalid arguments are rejected",
                    test_invalidArgumentsRejected) == NULL))
    {
        return -1;
    }

    return 0;
}
