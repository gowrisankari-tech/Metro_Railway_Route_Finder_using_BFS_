#include <stdio.h>
#include <string.h>

#include "fileio.h"
#include "../logger/logger.h"

/* File format (simple, human-readable, easy to error-check):

   STATIONS <count>
   <id> <name with underscores instead of spaces> <networkType>
   ...
   CONNECTIONS <count>
   <src_id> <dest_id>
   ...

   networkType is 0=Metro, 1=Railway, 2=Both (see NetworkType in
   graph.h). Older files saved before this field existed are
   still readable - loadNetwork() defaults to Both (2) when the
   third field is missing on a line.

   Station names are written with spaces replaced by '_' since
   the loader reads whitespace-delimited tokens; this keeps the
   parser simple and bounded (no dynamic line-length handling)
   without needing a stateful multi-space codec.
*/

static void writeNameEncoded(FILE *fp, const char *name)
{
    for(size_t i = 0U; name[i] != '\0'; i++)
    {
        char c = name[i];
        fputc((c == ' ') ? '_' : c, fp);
    }
}

static void decodeName(char *name)
{
    for(size_t i = 0U; name[i] != '\0'; i++)
    {
        if(name[i] == '_')
        {
            name[i] = ' ';
        }
    }
}

int saveNetwork(Graph *graph, const char *filename)
{
    if((graph == NULL) || (filename == NULL))
    {
        logError("saveNetwork: NULL argument");
        return 0;
    }

    FILE *fp = fopen(filename, "w");

    if(fp == NULL)
    {
        printf("Error: could not open '%s' for writing.\n", filename);
        logError("saveNetwork: fopen failed");
        return 0;
    }

    /* Count active stations first so the header count is
       accurate even if some stations were removed. */
    uint32_t activeCount = 0U;

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 1U)
        {
            activeCount++;
        }
    }

    if(fprintf(fp, "STATIONS %u\n", activeCount) < 0)
    {
        logError("saveNetwork: write failed (stations header)");
        fclose(fp);
        return 0;
    }

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 1U)
        {
            fprintf(fp, "%u ", graph->stations[i].stationId);
            writeNameEncoded(fp, graph->stations[i].stationName);
            fprintf(fp, " %d\n", (int)graph->stations[i].networkType);
        }
    }

    /* Count each undirected connection once (adjacency list
       stores both directions). */
    uint32_t connCount = 0U;

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 0U)
        {
            continue;
        }

        Edge *e = graph->adjList[i];

        while(e != NULL)
        {
            if((uint32_t)e->destination > i)
            {
                connCount++;
            }

            e = e->next;
        }
    }

    if(fprintf(fp, "CONNECTIONS %u\n", connCount) < 0)
    {
        logError("saveNetwork: write failed (connections header)");
        fclose(fp);
        return 0;
    }

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(graph->stations[i].active == 0U)
        {
            continue;
        }

        Edge *e = graph->adjList[i];

        while(e != NULL)
        {
            if((uint32_t)e->destination > i)
            {
                fprintf(fp, "%u %d\n", i, e->destination);
            }

            e = e->next;
        }
    }

    if(fclose(fp) != 0)
    {
        logWarning("saveNetwork: fclose reported an error");
        printf("Warning: network saved, but the file may not have flushed cleanly.\n");
        return 0;
    }

    printf("Network saved to '%s' (%u stations, %u connections).\n",
           filename, activeCount, connCount);
    logInfo("saveNetwork: completed successfully");
    return 1;
}

int loadNetwork(Graph *graph, const char *filename)
{
    if((graph == NULL) || (filename == NULL))
    {
        logError("loadNetwork: NULL argument");
        return 0;
    }

    FILE *fp = fopen(filename, "r");

    if(fp == NULL)
    {
        printf("Error: could not open '%s' for reading. Has it been saved yet?\n", filename);
        logError("loadNetwork: fopen failed");
        return 0;
    }

    char tag[16];
    unsigned int stationCount = 0U;

    if(fscanf(fp, "%15s %u", tag, &stationCount) != 2 ||
       strcmp(tag, "STATIONS") != 0)
    {
        printf("Error: '%s' is not a valid network file.\n", filename);
        logError("loadNetwork: malformed STATIONS header");
        fclose(fp);
        return 0;
    }

    /* Maps file-local station ids -> ids in this graph, since
       loading on top of an existing graph means ids can shift. */
    int idMap[MAX_STATIONS];

    for(unsigned int i = 0U; i < MAX_STATIONS; i++)
    {
        idMap[i] = -1;
    }

    unsigned int loadedStations = 0U;

    for(unsigned int i = 0U; i < stationCount; i++)
    {
        unsigned int fileId = 0U;
        char rawName[NAME_LEN];
        int typeValue = (int)NETWORK_BOTH;

        int matched = fscanf(fp, "%u %49s %d", &fileId, rawName, &typeValue);

        if(matched < 2)
        {
            logWarning("loadNetwork: skipped malformed station line");
            continue;
        }

        if(matched == 2)
        {
            /* Older network.dat files saved before networkType
               was added won't have a third field - default to
               NETWORK_BOTH so those stations stay searchable
               from both Metro and Railway, matching their
               pre-existing behavior. */
            typeValue = (int)NETWORK_BOTH;
        }

        NetworkType type = NETWORK_BOTH;

        if((typeValue == (int)NETWORK_METRO) || (typeValue == (int)NETWORK_RAILWAY))
        {
            type = (NetworkType)typeValue;
        }

        decodeName(rawName);

        int newId = addStation(graph, rawName, type);

        if(newId == -1)
        {
            logWarning("loadNetwork: addStation failed (limit reached?)");
            continue;
        }

        if(fileId < MAX_STATIONS)
        {
            idMap[fileId] = newId;
        }

        loadedStations++;
    }

    char connTag[16];
    unsigned int connCount = 0U;

    if(fscanf(fp, "%15s %u", connTag, &connCount) != 2 ||
       strcmp(connTag, "CONNECTIONS") != 0)
    {
        printf("Warning: network file had no valid CONNECTIONS section; stations were still loaded.\n");
        logWarning("loadNetwork: malformed CONNECTIONS header");
        fclose(fp);
        return (loadedStations > 0U) ? 1 : 0;
    }

    unsigned int loadedConnections = 0U;

    for(unsigned int i = 0U; i < connCount; i++)
    {
        unsigned int a = 0U;
        unsigned int b = 0U;

        if(fscanf(fp, "%u %u", &a, &b) != 2)
        {
            logWarning("loadNetwork: skipped malformed connection line");
            continue;
        }

        if((a < MAX_STATIONS) && (b < MAX_STATIONS) &&
           (idMap[a] != -1) && (idMap[b] != -1))
        {
            addConnection(graph, idMap[a], idMap[b]);
            loadedConnections++;
        }
        else
        {
            logWarning("loadNetwork: connection referenced unknown station id");
        }
    }

    if(fclose(fp) != 0)
    {
        logWarning("loadNetwork: fclose reported an error");
    }

    printf("Network loaded from '%s' (%u stations, %u connections).\n",
           filename, loadedStations, loadedConnections);
    logInfo("loadNetwork: completed");

    return (loadedStations > 0U) ? 1 : 0;
}
