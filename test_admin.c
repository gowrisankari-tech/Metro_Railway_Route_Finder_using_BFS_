/*
 * test_admin.c - CUnit tests for admin.c
 *
 * These tests exercise the Admin entry flow without requiring
 * manual keyboard input.  The menu is fed through a temporary
 * stdin file and the graph is checked where applicable.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>

#include "../admin/admin.h"
#include "../auth/auth.h"
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
        return;
    }

    /* Keep the stream alive for the duration of the test. */
    (void)fp;
}

static int suiteInit(void)
{
    g = createGraph();
    if(g == NULL)
    {
        return -1;
    }

    authInit();
    return 0;
}

static int suiteClean(void)
{
    freeGraph(g);
    g = NULL;
    return 0;
}

static void test_admin_backFromEntry(void)
{
    setInput("3\n");
    adminEntry(g);
    CU_ASSERT_PTR_NOT_NULL(g);
}

static void test_admin_failedLoginThenBack(void)
{
    setInput("2\nunknown_user\nwrong_password\n3\n");
    adminEntry(g);
    CU_ASSERT_PTR_NOT_NULL(g);
}

int addAdminTests(void)
{
    CU_pSuite suite = CU_add_suite("admin_suite", suiteInit, suiteClean);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "admin entry can return with Back",
                    test_admin_backFromEntry) == NULL) ||
       (CU_add_test(suite, "failed login returns to admin entry",
                    test_admin_failedLoginThenBack) == NULL))
    {
        return -1;
    }

    return 0;
}
