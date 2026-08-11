#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#define MAX_STATIONS 200U
#define NAME_LEN 50U

/* =====================================
   Structure Padding Demonstration
   ===================================== */

typedef struct
{
    char lineCode;
    int stationId;
    char active;
} NormalStation;

#pragma pack(push, 1)

typedef struct
{
    char lineCode;
    int stationId;
    char active;
} PackedStation;

#pragma pack(pop)

/* =====================================
   Actual Project Structures
   ===================================== */

/* Every station is tagged with which network(s) it belongs to.
   This replaces an earlier hardcoded name-list approach in
   user.c that silently rejected any station Admin added on the
   fly (e.g. new states/cities) since the list never knew about
   them - stations are now searchable in Metro/Railway based on
   this real, persisted tag instead of a fixed array. */
typedef enum
{
    NETWORK_METRO = 0,
    NETWORK_RAILWAY = 1,
    NETWORK_BOTH = 2
} NetworkType;

typedef struct
{
    uint32_t stationId;
    char stationName[NAME_LEN];
    uint8_t active;
    NetworkType networkType;
} Station;

typedef struct Edge
{
    int destination;
    struct Edge *next;
} Edge;

typedef struct
{
    uint32_t vertices;
    Station stations[MAX_STATIONS];
    Edge *adjList[MAX_STATIONS];
} Graph;

/* =====================================
   Graph Functions
   ===================================== */

Graph *createGraph(void);

int addStation(Graph *graph, const char *name, NetworkType type);

void removeStation(Graph *graph, int stationId);

void addConnection(Graph *graph, int src, int dest);

/* Returns 1 if an edge existed and was removed, 0 if no such
   edge was found (used so callers can report accurate status
   instead of an unconditional success message). */
int removeConnection(Graph *graph, int src, int dest);

int findStation(Graph *graph, const char *name);

/* Returns 1 if station index i is a valid, active station. */
int isActiveStation(const Graph *graph, int i);

/* Returns 1 if station index i is active AND belongs to the
   requested network (or is tagged NETWORK_BOTH). */
int isStationInNetwork(const Graph *graph, int i, NetworkType type);

void displayStations(Graph *graph);

void displayGraph(Graph *graph);

void freeGraph(Graph *graph);

#endif
