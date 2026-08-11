#ifndef UTILS_H
#define UTILS_H

#include "../graph/graph.h"

void clearInputBuffer(void);

/* Reads one integer menu choice within [minChoice, maxChoice].
   Checks scanf's return value (fixes prior finding of
   unchecked scanf across every menu loop): on non-numeric
   input, or a value outside range, it clears the input buffer,
   prints a message, and re-prompts - it never returns an
   invalid or stale value. */
int readMenuChoice(int minChoice, int maxChoice);

/* Reads a line of input safely (bounded by size), strips the
   trailing newline. Wraps fgets so every call site doesn't
   repeat the same boilerplate. */
void readLineInput(char *buffer, size_t size);

void showAdminMenu(void);

void initializeDefaultNetwork(Graph *graph);

#endif
