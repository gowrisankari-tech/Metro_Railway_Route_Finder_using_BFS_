/*
 * test_validation.c - CUnit tests for validation.c
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "../validation/validation.h"
#include "../graph/graph.h"

static void test_validateMenuChoice_validRange(void)
{
    CU_ASSERT_EQUAL(validateMenuChoice(1, 1, 3), 1);
    CU_ASSERT_EQUAL(validateMenuChoice(3, 1, 3), 1);
}

static void test_validateMenuChoice_outOfRange(void)
{
    CU_ASSERT_EQUAL(validateMenuChoice(0, 1, 3), 0);
    CU_ASSERT_EQUAL(validateMenuChoice(4, 1, 3), 0);
}

static void test_validateStationName_validNames(void)
{
    CU_ASSERT_EQUAL(validateStationName("Chennai Central"), 1);
    CU_ASSERT_EQUAL(validateStationName("Station123"), 1);
    CU_ASSERT_EQUAL(validateStationName("A B C"), 1);
}

static void test_validateStationName_invalidNames(void)
{
    CU_ASSERT_EQUAL(validateStationName(NULL), 0);
    CU_ASSERT_EQUAL(validateStationName(""), 0);
    CU_ASSERT_EQUAL(validateStationName("Chennai@Central"), 0);
    CU_ASSERT_EQUAL(validateStationName("Station-1"), 0);
}

static void test_validateStationIndex_boundaries(void)
{
    CU_ASSERT_EQUAL(validateStationIndex(0), 1);
    CU_ASSERT_EQUAL(validateStationIndex((int)MAX_STATIONS - 1), 1);
}

static void test_validateStationIndex_invalid(void)
{
    CU_ASSERT_EQUAL(validateStationIndex(-1), 0);
    CU_ASSERT_EQUAL(validateStationIndex((int)MAX_STATIONS), 0);
}

int addValidationTests(void)
{
    CU_pSuite suite = CU_add_suite("validation_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "valid menu choices accepted",
                    test_validateMenuChoice_validRange) == NULL) ||
       (CU_add_test(suite, "out-of-range menu choices rejected",
                    test_validateMenuChoice_outOfRange) == NULL) ||
       (CU_add_test(suite, "valid station names accepted",
                    test_validateStationName_validNames) == NULL) ||
       (CU_add_test(suite, "invalid station names rejected",
                    test_validateStationName_invalidNames) == NULL) ||
       (CU_add_test(suite, "valid station indexes accepted",
                    test_validateStationIndex_boundaries) == NULL) ||
       (CU_add_test(suite, "invalid station indexes rejected",
                    test_validateStationIndex_invalid) == NULL))
    {
        return -1;
    }

    return 0;
}
