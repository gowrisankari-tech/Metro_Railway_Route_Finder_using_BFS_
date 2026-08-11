/*
 * test_bfs.c - CUnit tests for bfs.c
 *
 * Builds a small known graph:
 *
 *     A - B - C - D
 *      \_________/
 *
 * so the shortest A->D path is the direct edge (2 stations),
 * and with that edge blocked, the alternate route is
 * A-B-C-D (4 stations) - this is exactly the scenario
 * findShortestAndAlternate() is designed to find.
 */

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stddef.h>

#include "../graph/graph.h"
#include "../bfs/bfs.h"

static Graph *g;
static int a, b, c, d, isolated;

static int suiteInit(void)
{
    g = createGraph();

    if(g == NULL)
    {
        return -1;
    }

    a = addStation(g, "A", NETWORK_BOTH);
    b = addStation(g, "B", NETWORK_BOTH);
    c = addStation(g, "C", NETWORK_BOTH);
    d = addStation(g, "D", NETWORK_BOTH);
    isolated = addStation(g, "Isolated", NETWORK_BOTH);

    addConnection(g, a, b);
    addConnection(g, b, c);
    addConnection(g, c, d);
    addConnection(g, a, d); /* shortcut edge */

    return 0;
}

static int suiteClean(void)
{
    freeGraph(g);
    g = NULL;
    return 0;
}

static void test_findShortestPath_usesDirectEdge(void)
{
    int path[MAX_STATIONS];
    int len = findShortestPath(g, a, d, path);

    CU_ASSERT_EQUAL(len, 2);
    CU_ASSERT_EQUAL(path[0], a);
    CU_ASSERT_EQUAL(path[1], d);
}

static void test_findShortestPath_sameSourceAndDest(void)
{
    int path[MAX_STATIONS];
    int len = findShortestPath(g, a, a, path);

    CU_ASSERT_EQUAL(len, 1);
    CU_ASSERT_EQUAL(path[0], a);
}

static void test_findShortestPath_unreachableReturnsZero(void)
{
    int path[MAX_STATIONS];
    int len = findShortestPath(g, a, isolated, path);

    CU_ASSERT_EQUAL(len, 0);
}

static void test_findShortestPath_skipsRemovedStation(void)
{
    /* Regression test for the fix where BFS previously ignored
       the active flag entirely: after removing B, a path that
       depends on B must no longer be usable. */
    Graph *g2 = createGraph();
    int x = addStation(g2, "X", NETWORK_BOTH);
    int y = addStation(g2, "Y", NETWORK_BOTH);
    int z = addStation(g2, "Z", NETWORK_BOTH);

    addConnection(g2, x, y);
    addConnection(g2, y, z);

    removeStation(g2, y);

    int path[MAX_STATIONS];
    int len = findShortestPath(g2, x, z, path);

    CU_ASSERT_EQUAL(len, 0); /* no route now that Y is inactive */

    freeGraph(g2);
}

static void test_findShortestAndAlternate_doesNotCrash(void)
{
    /* findShortestAndAlternate() only prints - this test
       exercises the full shortest+alternate code path
       (including the blocked-edge re-search) to catch
       crashes/asserts; output correctness for the shortest
       leg is already covered above via findShortestPath(). */
    findShortestAndAlternate(g, a, d);
    CU_ASSERT_TRUE(1);
}

int addBfsTests(void)
{
    CU_pSuite suite = CU_add_suite("bfs_suite", suiteInit, suiteClean);

    if(suite == NULL)
    {
        return -1;
    }

    if((CU_add_test(suite, "findShortestPath takes the direct edge", test_findShortestPath_usesDirectEdge) == NULL) ||
       (CU_add_test(suite, "findShortestPath handles source == destination", test_findShortestPath_sameSourceAndDest) == NULL) ||
       (CU_add_test(suite, "findShortestPath returns 0 for unreachable station", test_findShortestPath_unreachableReturnsZero) == NULL) ||
       (CU_add_test(suite, "findShortestPath skips a removed (inactive) station", test_findShortestPath_skipsRemovedStation) == NULL) ||
       (CU_add_test(suite, "findShortestAndAlternate runs without crashing", test_findShortestAndAlternate_doesNotCrash) == NULL))
    {
        return -1;
    }

    return 0;
}
