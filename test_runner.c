/*
 * test_runner.c - registers every suite and runs them via
 * CUnit's Basic interface.
 *
 * Build/run instructions: see tests/README.md
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>

#include "../logger/logger.h"
#include "../memory/memory.h"

int addGraphTests(void);
int addQueueTests(void);
int addBfsTests(void);
int addAuthTests(void);
int addStrutilsTests(void);
int addTrainTests(void);

int main(void)
{
    memoryInit();

    if(CU_initialize_registry() != CUE_SUCCESS)
    {
        return (int)CU_get_error();
    }

    if((addGraphTests() != 0) ||
       (addQueueTests() != 0) ||
       (addBfsTests() != 0) ||
       (addAuthTests() != 0) ||
       (addStrutilsTests() != 0) ||
       (addTrainTests() != 0))
    {
        fprintf(stderr, "Failed to register one or more test suites.\n");
        CU_cleanup_registry();
        return (int)CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    unsigned int failures = CU_get_number_of_failures();

    CU_cleanup_registry();

    return (failures > 0U) ? 1 : 0;
}
