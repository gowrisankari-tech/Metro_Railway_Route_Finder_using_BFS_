#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/graph.h"
#include "utils/utils.h"
#include "logger/logger.h"
#include "memory/memory.h"
#include "auth/auth.h"
#include "admin/admin.h"
#include "user/user.h"

int main(void)
{
    Graph *graph;

    memoryInit();

    graph = createGraph();

    if(graph == NULL)
    {
        printf("Graph Creation Failed\n");
        return 1;
    }

    logInfo("Application Started");

    initializeDefaultNetwork(graph);
    authInit();

    printf("\n");
    printf("====================================\n");
    printf("STRUCTURE PADDING DEMONSTRATION\n");
    printf("====================================\n");

    printf("Normal Structure Size : %zu Bytes\n", sizeof(NormalStation));
    printf("Packed Structure Size : %zu Bytes\n", sizeof(PackedStation));
    printf("Station Structure Size: %zu Bytes\n", sizeof(Station));

    printf("====================================\n");

    while(1)
    {
        printf("\n");
        printf("=====================================\n");
        printf(" METRO RAILWAY ROUTE FINDER\n");
        printf("=====================================\n");

        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. Exit\n");

        printf("=====================================\n");

        int choice = readMenuChoice(1, 3);

        switch(choice)
        {
            case 1:

                adminEntry(graph);
                break;

            case 2:

                userMenu(graph);
                break;

            case 3:

                printf("\nClosing Application...\n");
                freeGraph(graph);
                displayMemoryStatistics();
                logInfo("Application Closed");
                printf("\nProgram Terminated Successfully\n");
                return 0;

            default:

                printf("\nInvalid Choice\n");
                logWarning("Invalid Main Menu Choice");
        }
    }

    return 0;
}
