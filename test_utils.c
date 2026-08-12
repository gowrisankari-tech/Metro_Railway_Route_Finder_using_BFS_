/*
 * test_utils.c - CUnit tests for utils.c
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <unistd.h>

#include "../utils/utils.h"

static void redirectInput(const char *text)
{
    FILE *fp = tmpfile();
    CU_ASSERT_PTR_NOT_NULL_FATAL(fp);

    fputs(text, fp);
    rewind(fp);

    if(dup2(fileno(fp), fileno(stdin)) == -1)
    {
        fclose(fp);
        CU_FAIL("Could not redirect stdin");
    }
}

static void test_readLineInput_readsAndStripsNewline(void)
{
    char buffer[32];

    redirectInput("Chennai Central\n");
    readLineInput(buffer, sizeof(buffer));

    CU_ASSERT_STRING_EQUAL(buffer, "Chennai Central");
}

static void test_readLineInput_nullArguments_safe(void)
{
    char buffer[8] = "abc";

    readLineInput(NULL, sizeof(buffer));
    readLineInput(buffer, 0U);

    CU_ASSERT_STRING_EQUAL(buffer, "abc");
}

static void test_readMenuChoice_acceptsValidValue(void)
{
    redirectInput("2\n");
    CU_ASSERT_EQUAL(readMenuChoice(1, 3), 2);
}

static void test_readMenuChoice_retriesInvalidValue(void)
{
    redirectInput("bad\n3\n");
    CU_ASSERT_EQUAL(readMenuChoice(1, 3), 3);
}

int addUtilsTests(void)
{
    CU_pSuite suite = CU_add_suite("utils_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "readLineInput strips newline",
                    test_readLineInput_readsAndStripsNewline) == NULL) ||
       (CU_add_test(suite, "readLineInput handles NULL/zero size safely",
                    test_readLineInput_nullArguments_safe) == NULL) ||
       (CU_add_test(suite, "readMenuChoice accepts valid input",
                    test_readMenuChoice_acceptsValidValue) == NULL) ||
       (CU_add_test(suite, "readMenuChoice retries invalid input",
                    test_readMenuChoice_retriesInvalidValue) == NULL))
    {
        return -1;
    }

    return 0;
}
