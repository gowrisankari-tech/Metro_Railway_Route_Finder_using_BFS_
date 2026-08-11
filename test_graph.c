/*
 * test_graph.c - CUnit tests for graph.c
 *
 * Build: see tests/README.md (requires libcunit1-dev)
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stddef.h>

#include "../graph/graph.h"

static Graph *g;

static int suiteInit(void)
{
    g = createGraph();
    return (g == NULL) ? -1 : 0;
}

static int suiteClean(void)
{
    freeGraph(g);
    g = NULL;
    return 0;
}

static void test_createGraph_notNull(void)
{
    CU_ASSERT_PTR_NOT_NULL(g);
    CU_ASSERT_EQUAL(g->vertices, 0U);
}

static void test_addStation_returnsIncrementingIds(void)
{
    int a = addStation(g, "Alpha", NETWORK_BOTH);
    int b = addStation(g, "Beta", NETWORK_BOTH);

    CU_ASSERT_EQUAL(a, 0);
    CU_ASSERT_EQUAL(b, 1);
    CU_ASSERT_EQUAL(g->vertices, 2U);
}

static void test_addStation_nullArgumentsRejected(void)
{
    CU_ASSERT_EQUAL(addStation(NULL, "X", NETWORK_BOTH), -1);
    CU_ASSERT_EQUAL(addStation(g, NULL, NETWORK_BOTH), -1);
}

static void test_findStation_findsExisting(void)
{
    int id = addStation(g, "Gamma", NETWORK_BOTH);
    CU_ASSERT_EQUAL(findStation(g, "Gamma"), id);
}

static void test_findStation_missingReturnsMinusOne(void)
{
    CU_ASSERT_EQUAL(findStation(g, "DoesNotExist"), -1);
}

static void test_findStation_isCaseInsensitive(void)
{
    /* Regression test: findStation() previously used strcmp(),
       so "CHENNAI" would not match a station stored as
       "Chennai". It now matches regardless of case. */
    int id = addStation(g, "Chennai", NETWORK_RAILWAY);

    CU_ASSERT_EQUAL(findStation(g, "CHENNAI"), id);
    CU_ASSERT_EQUAL(findStation(g, "chennai"), id);
    CU_ASSERT_EQUAL(findStation(g, "ChEnNaI"), id);
}

static void test_addConnection_and_isActiveStation(void)
{
    int a = addStation(g, "Delta", NETWORK_BOTH);
    int b = addStation(g, "Epsilon", NETWORK_BOTH);

    addConnection(g, a, b);

    CU_ASSERT_EQUAL(isActiveStation(g, a), 1);
    CU_ASSERT_EQUAL(isActiveStation(g, b), 1);

    /* adjacency should be bidirectional */
    int found = 0;
    Edge *e = g->adjList[a];
    while(e != NULL)
    {
        if(e->destination == b) { found = 1; }
        e = e->next;
    }
    CU_ASSERT_EQUAL(found, 1);
}

static void test_removeConnection_existingReturnsOne(void)
{
    int a = addStation(g, "Zeta", NETWORK_BOTH);
    int b = addStation(g, "Eta", NETWORK_BOTH);

    addConnection(g, a, b);

    CU_ASSERT_EQUAL(removeConnection(g, a, b), 1);
}

static void test_removeConnection_missingReturnsZero(void)
{
    int a = addStation(g, "Theta", NETWORK_BOTH);
    int b = addStation(g, "Iota", NETWORK_BOTH);

    /* never connected */
    CU_ASSERT_EQUAL(removeConnection(g, a, b), 0);
}

static void test_removeStation_marksInactive(void)
{
    int a = addStation(g, "Kappa", NETWORK_BOTH);

    CU_ASSERT_EQUAL(isActiveStation(g, a), 1);

    removeStation(g, a);

    CU_ASSERT_EQUAL(isActiveStation(g, a), 0);

    /* removed station should no longer be findable by name */
    CU_ASSERT_EQUAL(findStation(g, "Kappa"), -1);
}

static void test_isStationInNetwork_respectsTag(void)
{
    /* Regression test for the fix where Metro/Railway search
       used a hardcoded name list instead of each station's
       real networkType - any station Admin added dynamically
       (e.g. "Kerala", "Meghalaya") used to be invisible to
       search even though it existed in the graph. */
    int metroOnly = addStation(g, "MetroOnlyStation", NETWORK_METRO);
    int railwayOnly = addStation(g, "RailwayOnlyStation", NETWORK_RAILWAY);
    int both = addStation(g, "BothStation", NETWORK_BOTH);

    CU_ASSERT_EQUAL(isStationInNetwork(g, metroOnly, NETWORK_METRO), 1);
    CU_ASSERT_EQUAL(isStationInNetwork(g, metroOnly, NETWORK_RAILWAY), 0);

    CU_ASSERT_EQUAL(isStationInNetwork(g, railwayOnly, NETWORK_RAILWAY), 1);
    CU_ASSERT_EQUAL(isStationInNetwork(g, railwayOnly, NETWORK_METRO), 0);

    CU_ASSERT_EQUAL(isStationInNetwork(g, both, NETWORK_METRO), 1);
    CU_ASSERT_EQUAL(isStationInNetwork(g, both, NETWORK_RAILWAY), 1);
}

int addGraphTests(void)
{
    CU_pSuite suite = CU_add_suite("graph_suite", suiteInit, suiteClean);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "createGraph returns valid empty graph", test_createGraph_notNull) == NULL) ||
       (CU_add_test(suite, "addStation returns incrementing ids", test_addStation_returnsIncrementingIds) == NULL) ||
       (CU_add_test(suite, "addStation rejects NULL arguments", test_addStation_nullArgumentsRejected) == NULL) ||
       (CU_add_test(suite, "findStation finds an existing station", test_findStation_findsExisting) == NULL) ||
       (CU_add_test(suite, "findStation returns -1 for missing station", test_findStation_missingReturnsMinusOne) == NULL) ||
       (CU_add_test(suite, "findStation is case-insensitive", test_findStation_isCaseInsensitive) == NULL) ||
       (CU_add_test(suite, "addConnection creates bidirectional edge", test_addConnection_and_isActiveStation) == NULL) ||
       (CU_add_test(suite, "removeConnection returns 1 when edge existed", test_removeConnection_existingReturnsOne) == NULL) ||
       (CU_add_test(suite, "removeConnection returns 0 when edge did not exist", test_removeConnection_missingReturnsZero) == NULL) ||
       (CU_add_test(suite, "removeStation marks station inactive", test_removeStation_marksInactive) == NULL) ||
       (CU_add_test(suite, "isStationInNetwork respects each station's network tag", test_isStationInNetwork_respectsTag) == NULL))
    {
        return -1;
    }

    return 0;
}
