/*
 * test_fileio.c - CUnit tests for fileio.c
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "../fileio/fileio.h"

static const char *testFile = "cunit_network_test.dat";

static void removeTestFile(void)
{
    (void)remove(testFile);
}

static void test_saveAndLoad_preservesStationsAndConnection(void)
{
    Graph *g1 = createGraph();
    Graph *g2 = createGraph();

    CU_ASSERT_PTR_NOT_NULL_FATAL(g1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g2);

    int a = addStation(g1, "Chennai Central", NETWORK_RAILWAY);
    int b = addStation(g1, "Bengaluru", NETWORK_RAILWAY);
    CU_ASSERT_NOT_EQUAL(a, -1);
    CU_ASSERT_NOT_EQUAL(b, -1);

    addConnection(g1, a, b);

    CU_ASSERT_EQUAL(saveNetwork(g1, testFile), 1);
    CU_ASSERT_EQUAL(loadNetwork(g2, testFile), 1);

    int loadedA = findStation(g2, "Chennai Central");
    int loadedB = findStation(g2, "Bengaluru");

    CU_ASSERT_NOT_EQUAL(loadedA, -1);
    CU_ASSERT_NOT_EQUAL(loadedB, -1);
    CU_ASSERT_TRUE(isStationInNetwork(g2, loadedA, NETWORK_RAILWAY));
    CU_ASSERT_TRUE(isStationInNetwork(g2, loadedB, NETWORK_RAILWAY));

    freeGraph(g1);
    freeGraph(g2);
    removeTestFile();
}

static void test_saveNetwork_nullArguments_fail(void)
{
    Graph *g = createGraph();

    CU_ASSERT_PTR_NOT_NULL_FATAL(g);
    CU_ASSERT_EQUAL(saveNetwork(NULL, testFile), 0);
    CU_ASSERT_EQUAL(saveNetwork(g, NULL), 0);

    freeGraph(g);
}

static void test_loadNetwork_missingFile_fail(void)
{
    Graph *g = createGraph();

    CU_ASSERT_PTR_NOT_NULL_FATAL(g);
    removeTestFile();

    CU_ASSERT_EQUAL(loadNetwork(g, testFile), 0);

    freeGraph(g);
}

static void test_loadNetwork_malformedFile_fail(void)
{
    FILE *fp = fopen(testFile, "w");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fp);

    fputs("NOT_A_NETWORK_FILE\n", fp);
    fclose(fp);

    Graph *g = createGraph();
    CU_ASSERT_PTR_NOT_NULL_FATAL(g);

    CU_ASSERT_EQUAL(loadNetwork(g, testFile), 0);

    freeGraph(g);
    removeTestFile();
}

int addFileioTests(void)
{
    CU_pSuite suite = CU_add_suite("fileio_suite", NULL, NULL);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "save and load preserve network data",
                    test_saveAndLoad_preservesStationsAndConnection) == NULL) ||
       (CU_add_test(suite, "save rejects NULL arguments",
                    test_saveNetwork_nullArguments_fail) == NULL) ||
       (CU_add_test(suite, "load missing file fails cleanly",
                    test_loadNetwork_missingFile_fail) == NULL) ||
       (CU_add_test(suite, "load malformed file fails cleanly",
                    test_loadNetwork_malformedFile_fail) == NULL))
    {
        return -1;
    }

    return 0;
}
