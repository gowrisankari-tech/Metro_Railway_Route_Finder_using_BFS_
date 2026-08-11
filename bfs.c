#include <stdio.h>
#include <time.h>

#include "bfs.h"
#include "../queue/queue.h"
#include "../logger/logger.h"

/* ---------------------------------------------------------
   Core BFS traversal.

   If blockA/blockB are both >= 0, the single edge between
   them (in either direction) is skipped during traversal -
   this lets findShortestAndAlternate() search for a second
   route without mutating the graph (no addConnection /
   removeConnection malloc/free churn).

   Inactive stations (removed via Admin) are never visited,
   which fixes the earlier gap where BFS ignored the active
   flag entirely.

   Returns 1 if destination was reached, 0 otherwise. parent[]
   must be an array of at least MAX_STATIONS ints; it is fully
   initialized by this function.
   --------------------------------------------------------- */
static int bfsCore(Graph *graph, int source, int destination,
                    int parent[], int blockA, int blockB)
{
    if(graph == NULL)
    {
        return 0;
    }

    if((isActiveStation(graph, source) == 0) ||
       (isActiveStation(graph, destination) == 0))
    {
        return 0;
    }

    int visited[MAX_STATIONS] = {0};

    for(int i = 0; i < (int)MAX_STATIONS; i++)
    {
        parent[i] = -1;
    }

    Queue q;

    initQueue(&q);

    visited[source] = 1;

    enqueue(&q, source);

    while(!isEmpty(&q))
    {
        int current = dequeue(&q);

        if(current == destination)
        {
            return 1;
        }

        Edge *temp = graph->adjList[current];

        while(temp != NULL)
        {
            int neighbor = temp->destination;

            int isBlockedEdge =
                (((current == blockA) && (neighbor == blockB)) ||
                 ((current == blockB) && (neighbor == blockA))) ? 1 : 0;

            if((visited[neighbor] == 0) &&
               (isActiveStation(graph, neighbor) == 1) &&
               (isBlockedEdge == 0))
            {
                visited[neighbor] = 1;
                parent[neighbor] = current;
                enqueue(&q, neighbor);
            }

            temp = temp->next;
        }
    }

    return (visited[destination] == 1) ? 1 : 0;
}

static int reconstructPath(const int parent[], int destination, int path[])
{
    int reversed[MAX_STATIONS];
    int count = 0;

    int node = destination;

    while(node != -1)
    {
        reversed[count] = node;
        count++;
        node = parent[node];
    }

    for(int i = 0; i < count; i++)
    {
        path[i] = reversed[count - 1 - i];
    }

    return count;
}

void printPathArray(const int path[], int len, Graph *graph)
{
    if((graph == NULL) || (path == NULL))
    {
        return;
    }

    for(int i = 0; i < len; i++)
    {
        printf("%s\n", graph->stations[path[i]].stationName);
    }
}

/* Elaborated hop/interchange breakdown: instead of printing a
   bare "Number Of Hops/Interchanges : N", this prints each
   individual segment of the journey by name, e.g.:

     Hop 1 : Chennai -> Vijayawada
     Hop 2 : Vijayawada -> Nagpur
     Hop 3 : Nagpur -> Delhi

   so "3 hops" is immediately backed up by exactly which three
   station-to-station segments make up that count. */
static void printHopSegments(const int path[], int len, Graph *graph)
{
    if((graph == NULL) || (path == NULL))
    {
        return;
    }

    int hopCount = len - 1;

    printf("Number Of Hops/Interchanges : %d\n", hopCount);

    if(hopCount <= 0)
    {
        return;
    }

    printf("Route Segments:\n");

    for(int i = 0; i < hopCount; i++)
    {
        printf("  Hop %d : %s -> %s\n", i + 1,
               graph->stations[path[i]].stationName,
               graph->stations[path[i + 1]].stationName);
    }
}

int findShortestPath(Graph *graph, int source, int destination, int path[])
{
    int parent[MAX_STATIONS];

    if(bfsCore(graph, source, destination, parent, -1, -1) == 0)
    {
        return 0;
    }

    return reconstructPath(parent, destination, path);
}

void BFS(Graph *graph, int source, int destination)
{
    if(graph == NULL)
    {
        logError("NULL Graph Pointer");
        return;
    }

    clock_t startTime = clock();

    int path[MAX_STATIONS];

    logInfo("BFS Route Search Started");

    int len = findShortestPath(graph, source, destination, path);

    if(len == 0)
    {
        printf("\nNo Route Found\n");
        logError("Route Not Found");
        return;
    }

    printf("\n=========================\n");
    printf("SHORTEST ROUTE FOUND\n");
    printf("=========================\n\n");

    printPathArray(path, len, graph);

    printf("\nTotal Stations To Travel : %d\n", len);
    printHopSegments(path, len, graph);

    clock_t endTime = clock();

    double executionTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;

    printf("\nExecution Time : %.6f sec\n", executionTime);

    logInfo("Route Found Successfully");
}

void findShortestAndAlternate(Graph *graph, int source, int destination)
{
    if(graph == NULL)
    {
        logError("NULL Graph Pointer");
        return;
    }

    logInfo("Shortest + Alternate Route Search Started");

    /* --- Leg 1: shortest path (timed separately) --- */
    clock_t shortestStartTime = clock();

    int shortestPath[MAX_STATIONS];
    int shortestLen = findShortestPath(graph, source, destination, shortestPath);

    clock_t shortestEndTime = clock();
    double shortestExecutionTime =
        ((double)(shortestEndTime - shortestStartTime)) / CLOCKS_PER_SEC;

    if(shortestLen == 0)
    {
        printf("\nNo Route Found\n");
        printf("Shortest Path Execution Time : %.6f sec\n", shortestExecutionTime);
        logError("Route Not Found");
        return;
    }

    printf("\n=========================\n");
    printf("SHORTEST ROUTE\n");
    printf("=========================\n\n");

    printPathArray(shortestPath, shortestLen, graph);

    printf("\nTotal Stations To Travel : %d\n", shortestLen);
    printHopSegments(shortestPath, shortestLen, graph);
    printf("Shortest Path Execution Time : %.6f sec\n", shortestExecutionTime);

    /* --- Leg 2: alternate route (timed separately) ---
       Try excluding each edge of the shortest path in turn;
       keep the shortest successful result. No graph mutation
       occurs, so this costs zero extra malloc/free calls. */
    clock_t alternateStartTime = clock();

    int bestAltPath[MAX_STATIONS];
    int bestAltLen = 0;

    if(shortestLen >= 2)
    {
        for(int i = 0; i < (shortestLen - 1); i++)
        {
            int parent[MAX_STATIONS];
            int blockA = shortestPath[i];
            int blockB = shortestPath[i + 1];

            if(bfsCore(graph, source, destination, parent, blockA, blockB) == 1)
            {
                int candidatePath[MAX_STATIONS];
                int candidateLen = reconstructPath(parent, destination, candidatePath);

                if((bestAltLen == 0) || (candidateLen < bestAltLen))
                {
                    bestAltLen = candidateLen;

                    for(int k = 0; k < candidateLen; k++)
                    {
                        bestAltPath[k] = candidatePath[k];
                    }
                }
            }
        }
    }

    clock_t alternateEndTime = clock();
    double alternateExecutionTime =
        ((double)(alternateEndTime - alternateStartTime)) / CLOCKS_PER_SEC;

    printf("\n=========================\n");
    printf("ALTERNATE ROUTE\n");
    printf("=========================\n\n");

    if(bestAltLen > 0)
    {
        printPathArray(bestAltPath, bestAltLen, graph);
        printf("\nTotal Stations To Travel : %d\n", bestAltLen);
        printHopSegments(bestAltPath, bestAltLen, graph);
    }
    else
    {
        printf("No Alternate Route Found\n");
        logWarning("No alternate route exists");
    }

    printf("Alternate Path Execution Time : %.6f sec\n", alternateExecutionTime);
    printf("\nTotal Execution Time (Shortest + Alternate) : %.6f sec\n",
           shortestExecutionTime + alternateExecutionTime);

    logInfo("Shortest + Alternate Route Search Completed");
}
