/*
 * test_auth.c - CUnit tests for auth.c
 *
 * auth.c is used by Admin's Register/Login flow (User no longer
 * requires login). Tests here exercise the module directly and
 * are agnostic to which menu calls it.
 *
 * Note: authInit()/authRegister() touch users.dat on disk.
 * Tests run against a temporary working directory (see
 * tests/README.md) so they don't clobber a real users.dat.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stddef.h>
#include <string.h>

#include "../auth/auth.h"

static int suiteInit(void)
{
    authInit();
    return 0;
}

static void test_register_thenLogin_succeeds(void)
{
    CU_ASSERT_EQUAL(authRegister("cunit_user1", "pass123"), 1);
    CU_ASSERT_EQUAL(authLogin("cunit_user1", "pass123"), 1);
}

static void test_login_wrongPassword_fails(void)
{
    authRegister("cunit_user2", "correct");
    CU_ASSERT_EQUAL(authLogin("cunit_user2", "incorrect"), 0);
}

static void test_login_unknownUser_fails(void)
{
    CU_ASSERT_EQUAL(authLogin("no_such_user", "whatever"), 0);
}

static void test_register_duplicateUsername_rejected(void)
{
    CU_ASSERT_EQUAL(authRegister("cunit_dup", "first"), 1);
    CU_ASSERT_EQUAL(authRegister("cunit_dup", "second"), 0);

    /* original password should still be the one that works */
    CU_ASSERT_EQUAL(authLogin("cunit_dup", "first"), 1);
}

static void test_register_emptyFields_rejected(void)
{
    CU_ASSERT_EQUAL(authRegister("", "pass"), 0);
    CU_ASSERT_EQUAL(authRegister("user", ""), 0);
}

static void test_register_nullArguments_rejected(void)
{
    CU_ASSERT_EQUAL(authRegister(NULL, "pass"), 0);
    CU_ASSERT_EQUAL(authRegister("user", NULL), 0);
}

int addAuthTests(void)
{
    CU_pSuite suite = CU_add_suite("auth_suite", suiteInit, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "register then login succeeds", test_register_thenLogin_succeeds) == NULL) ||
       (CU_add_test(suite, "login with wrong password fails", test_login_wrongPassword_fails) == NULL) ||
       (CU_add_test(suite, "login with unknown user fails", test_login_unknownUser_fails) == NULL) ||
       (CU_add_test(suite, "duplicate username registration rejected", test_register_duplicateUsername_rejected) == NULL) ||
       (CU_add_test(suite, "empty username/password rejected", test_register_emptyFields_rejected) == NULL) ||
       (CU_add_test(suite, "NULL username/password rejected", test_register_nullArguments_rejected) == NULL))
    {
        return -1;
    }

    return 0;
}
