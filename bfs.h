#ifndef BFS_H
#define BFS_H

#include "../graph/graph.h"

/* Original simple interface (used by Admin's "Find Shortest
   Route" option): runs BFS and prints the route, station
   count, hop count, and execution time. Prints "No Route
   Found" if unreachable. */
void BFS(Graph *graph, int source, int destination);

/* Iterative path printer (no recursion, so stack depth does
   not grow with path length - see docs/MISRA_NOTES.md). */
void printPathArray(const int path[], int len, Graph *graph);

/* Computes the shortest path from source to destination.
   Writes station indices into path[] (caller-provided array
   of at least MAX_STATIONS ints) and returns the number of
   stations on the path, or 0 if no route exists. Skips
   inactive (removed) stations. */
int findShortestPath(Graph *graph, int source, int destination, int path[]);

/* Used for Metro/Railway "Find Shortest Route": prints the
   shortest path (with station count) and then searches for a
   genuine alternate route by re-running BFS with each edge of
   the shortest path individually excluded (no graph mutation,
   so no extra malloc/free churn), picking the shortest
   resulting alternate. Prints "No Alternate Route Found" if
   none exists. Prints total execution time at the end. */
void findShortestAndAlternate(Graph *graph, int source, int destination);

#endif
