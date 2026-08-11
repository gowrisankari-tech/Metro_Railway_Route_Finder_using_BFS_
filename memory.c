#include <stdio.h>

#include "memory.h"

static unsigned long totalAllocations = 0UL;
static unsigned long totalFrees = 0UL;
static long currentLive = 0L;
static long peakLive = 0L;

void memoryInit(void)
{
    totalAllocations = 0UL;
    totalFrees = 0UL;
    currentLive = 0L;
    peakLive = 0L;
}

void incrementAllocations(void)
{
    totalAllocations++;
    currentLive++;

    if(currentLive > peakLive)
    {
        peakLive = currentLive;
    }
}

void incrementFrees(void)
{
    totalFrees++;
    currentLive--;
}

void displayMemoryStatistics(void)
{
    printf("\n====================================\n");
    printf("MEMORY STATISTICS\n");
    printf("====================================\n");
    printf("Total Allocations   : %lu\n", totalAllocations);
    printf("Total Frees         : %lu\n", totalFrees);
    printf("Outstanding Blocks  : %ld%s\n", currentLive,
           (currentLive == 0L) ? " (clean - no leaks)" : " (POSSIBLE LEAK)");
    printf("Peak Live Blocks    : %ld\n", peakLive);
    printf("====================================\n");
}
