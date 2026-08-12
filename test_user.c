/*
 * test_user.c - CUnit tests for user.c
 *
 * userMenu() is interactive, so these tests exercise its public
 * entry point with scripted stdin and verify that it returns.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <unistd.h>

#include "../user/user.h"
#include "../graph/graph.h"

static Graph *g;

static void setInput(const char *text)
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

static int suiteInit(void)
{
    g = createGraph();
    if(g == NULL)
    {
        return -1;
    }

    return 0;
}

static int suiteClean(void)
{
    freeGraph(g);
    g = NULL;
    return 0;
}

static void test_userMenu_backReturns(void)
{
    setInput("3\n");
    userMenu(g);
    CU_ASSERT_PTR_NOT_NULL(g);
}

static void test_userMenu_metroThenBack(void)
{
    setInput("1\n3\n3\n");
    userMenu(g);
    CU_ASSERT_PTR_NOT_NULL(g);
}

static void test_userMenu_railwayThenBack(void)
{
    setInput("2\n3\n3\n");
    userMenu(g);
    CU_ASSERT_PTR_NOT_NULL(g);
}

int addUserTests(void)
{
    CU_pSuite suite = CU_add_suite("user_suite", suiteInit, suiteClean);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "user menu Back returns",
                    test_userMenu_backReturns) == NULL) ||
       (CU_add_test(suite, "user can enter Metro menu and return",
                    test_userMenu_metroThenBack) == NULL) ||
       (CU_add_test(suite, "user can enter Railway menu and return",
                    test_userMenu_railwayThenBack) == NULL))
    {
        return -1;
    }

    return 0;
}
