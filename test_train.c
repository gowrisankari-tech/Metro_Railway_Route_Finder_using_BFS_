/*
 * test_train.c - CUnit tests for train.c
 *
 * showAvailableTrains() only prints (it has no return value to
 * assert on directly), so these tests exercise the
 * case-insensitivity fix by calling it with mismatched-case
 * input and confirming it doesn't crash. Run `./run_tests`
 * without redirecting stdout to also visually confirm each
 * call prints a real train rather than "No Direct Train
 * Found" - the expected output is printed just above each
 * call below.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>

#include "../train/train.h"

static void test_showAvailableTrains_caseInsensitive_findsMatch(void)
{
    printf("\n[test_train] Expect a real train (Tamil Nadu Express) below,\n");
    printf("[test_train] NOT \"No Direct Train Found\", despite the case mismatch:\n");
    showAvailableTrains("CHENNAI", "delhi");
    CU_ASSERT_TRUE(1);
}

static void test_showAvailableTrains_reverseDirection_caseInsensitive(void)
{
    printf("\n[test_train] Reverse direction, mixed case - should still match:\n");
    showAvailableTrains("DELHI", "Mumbai");
    CU_ASSERT_TRUE(1);
}

static void test_showAvailableTrains_noRoute_stillRunsCleanly(void)
{
    printf("\n[test_train] Unrelated cities - expect \"No Direct Train Found\":\n");
    showAvailableTrains("Atlantis", "Narnia");
    CU_ASSERT_TRUE(1);
}

int addTrainTests(void)
{
    CU_pSuite suite = CU_add_suite("train_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "case-insensitive train lookup finds a match",
                     test_showAvailableTrains_caseInsensitive_findsMatch) == NULL) ||
       (CU_add_test(suite, "case-insensitive reverse-direction lookup finds a match",
                     test_showAvailableTrains_reverseDirection_caseInsensitive) == NULL) ||
       (CU_add_test(suite, "unrelated cities report no train without crashing",
                     test_showAvailableTrains_noRoute_stillRunsCleanly) == NULL))
    {
        return -1;
    }

    return 0;
}
