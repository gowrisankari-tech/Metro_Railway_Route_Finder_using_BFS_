#ifndef FILEIO_H
#define FILEIO_H

#include "../graph/graph.h"

#define NETWORK_FILE "network.dat"

/* Saves all active stations and connections to NETWORK_FILE.
   Returns 1 on success, 0 on any file-handling error (the
   specific error is logged via logger.c). */
int saveNetwork(Graph *graph, const char *filename);

/* Loads stations and connections from filename into graph.
   The graph must already exist (createGraph()); this ADDS
   the loaded stations/connections on top of whatever is
   already there. Returns 1 on success, 0 if the file could
   not be opened or contained no valid data. */
int loadNetwork(Graph *graph, const char *filename);

#endif
