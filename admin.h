#ifndef ADMIN_H
#define ADMIN_H

#include "../graph/graph.h"

/* Top-level entry point called from main.c for "Admin" - shows
   Register / Login / Back, and only reaches the Admin menu
   (adminMenu) after a successful login. */
void adminEntry(Graph *graph);

/* The actual Admin menu (View/Add/Remove/Save/Load/etc.),
   reached only after adminEntry() confirms a valid login. */
void adminMenu(Graph *graph);

#endif
