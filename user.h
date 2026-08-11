#ifndef USER_H
#define USER_H

#include "../graph/graph.h"

/* Top-level entry point called from main.c for "User" - goes
   straight to Metro/Railway/Exit, no login required. */
void userMenu(Graph *graph);

#endif
