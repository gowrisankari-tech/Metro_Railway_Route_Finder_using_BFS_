#ifndef MEMORY_H
#define MEMORY_H

void memoryInit(void);
void incrementAllocations(void);
void incrementFrees(void);

/* Prints total allocations/frees, current outstanding blocks,
   and the peak number of blocks that were ever live at once -
   the peak figure is what actually matters for memory
   optimization (it's the real worst-case footprint), not just
   the final allocations/frees totals. */
void displayMemoryStatistics(void);

#endif
