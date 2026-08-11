#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "../logger/logger.h"
#include "../memory/memory.h"
#include "../strutils/strutils.h"

Graph *createGraph(void)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));

    if(graph == NULL)
    {
        logError("Graph allocation failed");
        return NULL;
    }

    incrementAllocations();

    graph->vertices = 0U;

    for(uint32_t i = 0U; i < MAX_STATIONS; i++)
    {
        graph->adjList[i] = NULL;
    }

    logInfo("Graph created");

    return graph;
}

int addStation(Graph *graph, const char *name, NetworkType type)
{
    if((graph == NULL) || (name == NULL))
    {
        logError("addStation: NULL argument");
        return -1;
    }

    if(graph->vertices >= MAX_STATIONS)
    {
        logError("Maximum station limit reached");
        return -1;
    }

    uint32_t id = graph->vertices;

    graph->stations[id].stationId = id;

    strncpy(graph->stations[id].stationName, name, NAME_LEN - 1U);
    graph->stations[id].stationName[NAME_LEN - 1U] = '\0';

    graph->stations[id].active = 1U;
    graph->stations[id].networkType = type;
    graph->vertices++;

    logInfo("Station added");

    return (int)id;
}

void addConnection(Graph *graph, int src, int dest)
{
    if(graph == NULL)
    {
        return;
    }

    if((src < 0) || (src >= (int)graph->vertices) ||
       (dest < 0) || (dest >= (int)graph->vertices))
    {
        logError("addConnection: station index out of range");
        return;
    }

    Edge *newNode = (Edge *)malloc(sizeof(Edge));

    if(newNode == NULL)
    {
        logError("Edge allocation failed");
        return;
    }

    incrementAllocations();

    newNode->destination = dest;
    newNode->next = graph->adjList[src];

    Edge *reverseNode = (Edge *)malloc(sizeof(Edge));

    if(reverseNode == NULL)
    {
        /* Roll back the first allocation instead of leaving a
           one-directional edge half-linked into the graph. */
        free(newNode);
        incrementFrees();

        logError("Reverse edge allocation failed - connection not added");
        return;
    }

    incrementAllocations();

    /* Only link the forward edge in once we know both
       allocations succeeded. */
    graph->adjList[src] = newNode;

    reverseNode->destination = src;
    reverseNode->next = graph->adjList[dest];
    graph->adjList[dest] = reverseNode;

    logInfo("Connection added");
}

void removeStation(Graph *graph, int stationId)
{
    if(graph == NULL)
    {
        return;
    }

    if((stationId < 0) || (stationId >= (int)graph->vertices))
    {
        logWarning("removeStation: invalid station id");
        return;
    }

    graph->stations[stationId].active = 0U;

    printf("Station Removed Successfully\n");

    logInfo("Station Removed");
}

int removeConnection(Graph *graph, int src, int dest)
{
    if(graph == NULL)
    {
        return 0;
    }

    if((src < 0) || (src >= (int)graph->vertices) ||
       (dest < 0) || (dest >= (int)graph->vertices))
    {
        logWarning("removeConnection: invalid station index");
        return 0;
    }

    int removedForward = 0;
    int removedReverse = 0;

    Edge *curr = graph->adjList[src];
    Edge *prev = NULL;

    while(curr != NULL)
    {
        if(curr->destination == dest)
        {
            if(prev == NULL)
            {
                graph->adjList[src] = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }

            free(curr);
            incrementFrees();
            removedForward = 1;
            break;
        }

        prev = curr;
        curr = curr->next;
    }

    curr = graph->adjList[dest];
    prev = NULL;

    while(curr != NULL)
    {
        if(curr->destination == src)
        {
            if(prev == NULL)
            {
                graph->adjList[dest] = curr->next;
            }
            else
            {
                prev->next = curr->next;
            }

            free(curr);
            incrementFrees();
            removedReverse = 1;
            break;
        }

        prev = curr;
        curr = curr->next;
    }

    if((removedForward == 1) && (removedReverse == 1))
    {
        printf("Connection Removed Successfully\n");
        logInfo("Connection Removed");
        return 1;
    }

    printf("No Such Connection Exists\n");
    logWarning("removeConnection: connection not found");
    return 0;
}

int findStation(Graph *graph, const char *name)
{
    if((graph == NULL) || (name == NULL))
    {
        return -1;
    }

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if((graph->stations[i].active == 1U) &&
           (stringsEqualIgnoreCase(graph->stations[i].stationName, name) == 1))
        {
            return (int)i;
        }
    }

    return -1;
}

int isActiveStation(const Graph *graph, int i)
{
    if(graph == NULL)
    {
        return 0;
    }

    if((i < 0) || (i >= (int)graph->vertices))
    {
        return 0;
    }

    return (graph->stations[i].active == 1U) ? 1 : 0;
}

int isStationInNetwork(const Graph *graph, int i, NetworkType type)
{
    if(isActiveStation(graph, i) == 0)
    {
        return 0;
    }

    NetworkType stationType = graph->stations[i].networkType;

    if(stationType == NETWORK_BOTH)
    {
        return 1;
    }

    return (stationType == type) ? 1 : 0;
}

void displayStations(Graph *graph)
{
    if(graph == NULL)
    {
        return;
    }

    printf("\nStations:\n");

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 1U)
        {
            printf("%u. %s\n", graph->stations[i].stationId, graph->stations[i].stationName);
        }
    }
}

void displayGraph(Graph *graph)
{
    if(graph == NULL)
    {
        return;
    }

    printf("\n");
    printf("====================================\n");
    printf("NETWORK CONNECTIONS\n");
    printf("====================================\n\n");

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 0U)
        {
            continue;
        }

        printf("%s", graph->stations[i].stationName);

        Edge *temp = graph->adjList[i];

        while(temp != NULL)
        {
            if(isActiveStation(graph, temp->destination) == 1)
            {
                printf(" -> %s", graph->stations[temp->destination].stationName);
            }

            temp = temp->next;
        }

        printf("\n");
    }
}

void freeGraph(Graph *graph)
{
    if(graph == NULL)
    {
        return;
    }

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        Edge *temp = graph->adjList[i];

        while(temp != NULL)
        {
            Edge *next = temp->next;
            free(temp);
            incrementFrees();
            temp = next;
        }
    }

    free(graph);
    incrementFrees();

    logInfo("Graph memory released");
}
