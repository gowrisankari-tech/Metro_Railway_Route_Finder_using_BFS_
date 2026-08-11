#include <stdio.h>
#include <string.h>

#include "user.h"
#include "../bfs/bfs.h"
#include "../train/train.h"
#include "../utils/utils.h"
#include "../logger/logger.h"

/* =========================================================
   STATION CLASSIFICATION (Metro vs Railway)

   Previously this checked a hardcoded name list, which meant
   any station Admin added on the fly (e.g. a new state/city
   via "Add Connection") was invisible to Metro/Railway search
   even though it existed in the graph. Now it checks each
   station's real, persisted networkType tag instead - so
   anything Admin adds is searchable immediately, under
   whichever network it was tagged with when created.
   ========================================================= */

static void viewMetroStations(Graph *graph)
{
    printf("\n===== METRO STATIONS =====\n");

    int shown = 0;

    for(uint32_t i = 0U; i < graph->vertices; i++)
    {
        if(isStationInNetwork(graph, (int)i, NETWORK_METRO) == 1)
        {
            printf("%2u. %s\n", i, graph->stations[i].stationName);
            shown++;
        }
    }

    if(shown == 0)
    {
        printf("(No Metro stations yet - ask Admin to add one via \"Add Connection\".)\n");
    }
}

/* =========================================================
   METRO MENU
   ========================================================= */

static void metroMenu(Graph *graph)
{
    char source[NAME_LEN];
    char destination[NAME_LEN];

    while(1)
    {
        printf("\n===== METRO MENU =====\n");
        printf("1. View Metro Stations\n");
        printf("2. Find Shortest Route (with Alternate)\n");
        printf("3. Back\n");

        int choice = readMenuChoice(1, 3);

        switch(choice)
        {
            case 1:

                viewMetroStations(graph);
                break;

            case 2:
            {
                printf("Source Metro Station : ");
                readLineInput(source, sizeof(source));

                printf("Destination Metro Station : ");
                readLineInput(destination, sizeof(destination));

                int src = findStation(graph, source);
                int dest = findStation(graph, destination);

                if((src == -1) || (dest == -1))
                {
                    printf("\nStation Not Found In Network\n");
                    break;
                }

                if((isStationInNetwork(graph, src, NETWORK_METRO) == 0) ||
                   (isStationInNetwork(graph, dest, NETWORK_METRO) == 0))
                {
                    printf("\nThat station exists but isn't tagged as a Metro station.\n");
                    printf("(Choose option 1 to view valid Metro stations)\n");
                    break;
                }

                findShortestAndAlternate(graph, src, dest);

                break;
            }

            case 3:

                return;

            default:

                printf("\nInvalid Choice\n");
        }
    }
}

/* =========================================================
   RAILWAY MENU
   ========================================================= */

static void railwayMenu(Graph *graph)
{
    char source[NAME_LEN];
    char destination[NAME_LEN];

    while(1)
    {
        printf("\n===== RAILWAY MENU =====\n");
        printf("1. Find Shortest Route (with Alternate)\n");
        printf("2. View Available Trains\n");
        printf("3. Back\n");

        int choice = readMenuChoice(1, 3);

        switch(choice)
        {
            case 1:
            {
                printf("Source Station : ");
                readLineInput(source, sizeof(source));

                printf("Destination Station : ");
                readLineInput(destination, sizeof(destination));

                int src = findStation(graph, source);
                int dest = findStation(graph, destination);

                if((src == -1) || (dest == -1))
                {
                    printf("\nStation Not Found In Network\n");
                    break;
                }

                if((isStationInNetwork(graph, src, NETWORK_RAILWAY) == 0) ||
                   (isStationInNetwork(graph, dest, NETWORK_RAILWAY) == 0))
                {
                    printf("\nThat station exists but isn't tagged as a Railway station.\n");
                    break;
                }

                findShortestAndAlternate(graph, src, dest);

                break;
            }

            case 2:
            {
                printf("Source : ");
                readLineInput(source, sizeof(source));

                printf("Destination : ");
                readLineInput(destination, sizeof(destination));

                showAvailableTrains(source, destination);

                break;
            }

            case 3:

                return;

            default:

                printf("\nInvalid Choice\n");
        }
    }
}

/* =========================================================
   TOP-LEVEL ENTRY: Metro / Railway / Exit (no login required)
   ========================================================= */

void userMenu(Graph *graph)
{
    while(1)
    {
        printf("\n===== USER MENU =====\n");
        printf("1. Metro\n");
        printf("2. Railway\n");
        printf("3. Back to Main Menu\n");

        int choice = readMenuChoice(1, 3);

        switch(choice)
        {
            case 1:

                metroMenu(graph);
                break;

            case 2:

                railwayMenu(graph);
                break;

            case 3:

                return;

            default:

                printf("\nInvalid Choice\n");
        }
    }
}
