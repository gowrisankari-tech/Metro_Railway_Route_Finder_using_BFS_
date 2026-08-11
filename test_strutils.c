/*
 * test_strutils.c - CUnit tests for strutils.c
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stddef.h>

#include "../strutils/strutils.h"

static void test_equalStrings_sameCase(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("Chennai", "Chennai"), 1);
}

static void test_equalStrings_differentCase(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("CHENNAI", "chennai"), 1);
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("ChEnNaI", "cHeNnAi"), 1);
}

static void test_differentStrings_returnsZero(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("Chennai", "Delhi"), 0);
}

static void test_differentLengths_returnsZero(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("Chennai", "Chennai City"), 0);
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("Chen", "Chennai"), 0);
}

static void test_emptyStrings_equal(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("", ""), 1);
}

static void test_nullArguments_returnsZero(void)
{
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase(NULL, "Chennai"), 0);
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase("Chennai", NULL), 0);
    CU_ASSERT_EQUAL(stringsEqualIgnoreCase(NULL, NULL), 0);
}

int addStrutilsTests(void)
{
    CU_pSuite suite = CU_add_suite("strutils_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "identical strings match", test_equalStrings_sameCase) == NULL) ||
       (CU_add_test(suite, "different-case strings match", test_equalStrings_differentCase) == NULL) ||
       (CU_add_test(suite, "different strings do not match", test_differentStrings_returnsZero) == NULL) ||
       (CU_add_test(suite, "different-length strings do not match", test_differentLengths_returnsZero) == NULL) ||
       (CU_add_test(suite, "two empty strings match", test_emptyStrings_equal) == NULL) ||
       (CU_add_test(suite, "NULL arguments return 0, not a crash", test_nullArguments_returnsZero) == NULL))
    {
        return -1;
    }

    return 0;
}
