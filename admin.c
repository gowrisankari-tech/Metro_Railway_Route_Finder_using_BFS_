#include <stdio.h>
#include <string.h>

#include "admin.h"
#include "../bfs/bfs.h"
#include "../utils/utils.h"
#include "../validation/validation.h"
#include "../fileio/fileio.h"
#include "../auth/auth.h"
#include "../logger/logger.h"

static void promptStationName(const char *label, char *buffer, size_t size)
{
    printf("%s : ", label);
    readLineInput(buffer, size);
}

static NetworkType promptNetworkType(const char *stationName)
{
    printf("\n'%s' is a new station. Which network does it belong to?\n", stationName);
    printf("1. Metro\n");
    printf("2. Railway\n");
    printf("3. Both\n");

    int choice = readMenuChoice(1, 3);

    switch(choice)
    {
        case 1:  return NETWORK_METRO;
        case 2:  return NETWORK_RAILWAY;
        default: return NETWORK_BOTH;
    }
}

void adminEntry(Graph *graph)
{
    char username[AUTH_FIELD_LEN];
    char password[AUTH_FIELD_LEN];

    while(1)
    {
        printf("\n===== ADMIN ACCESS =====\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Back to Main Menu\n");

        int choice = readMenuChoice(1, 3);

        switch(choice)
        {
            case 1:
            {
                printf("Choose Admin Username : ");
                readLineInput(username, sizeof(username));

                printf("Choose Admin Password : ");
                readLineInput(password, sizeof(password));

                if(authRegister(username, password) == 1)
                {
                    printf("\nAdmin Registration Successful. Please log in.\n");
                    logInfo("Admin registration successful");
                }

                break;
            }

            case 2:
            {
                printf("Admin Username : ");
                readLineInput(username, sizeof(username));

                printf("Admin Password : ");
                readLineInput(password, sizeof(password));

                if(authLogin(username, password) == 1)
                {
                    printf("\nLogin Successful\n");
                    logInfo("Admin login successful");
                    adminMenu(graph);
                }
                else
                {
                    printf("\nLogin Failed: Incorrect Username Or Password.\n");
                    logWarning("Admin login failed - incorrect username or password");
                }

                break;
            }

            case 3:

                return;

            default:

                printf("\nInvalid Choice\n");
        }
    }
}

void adminMenu(Graph *graph)
{
    char source[NAME_LEN];
    char destination[NAME_LEN];

    while(1)
    {
        showAdminMenu();

        int choice = readMenuChoice(1, 8);

        switch(choice)
        {
            case 1:

                displayStations(graph);
                break;

            case 2:

                displayGraph(graph);
                break;

            case 3:
            {
                promptStationName("Source Station", source, sizeof(source));
                promptStationName("Destination Station", destination, sizeof(destination));

                if((validateStationName(source) == 0U) ||
                   (validateStationName(destination) == 0U))
                {
                    printf("Invalid station name (letters, numbers, spaces only).\n");
                    break;
                }

                int src = findStation(graph, source);
                int dest = findStation(graph, destination);

                if(src == -1)
                {
                    src = addStation(graph, source, promptNetworkType(source));
                }

                if(dest == -1)
                {
                    dest = addStation(graph, destination, promptNetworkType(destination));
                }

                if((src == -1) || (dest == -1))
                {
                    printf("Could not create/find one of the stations.\n");
                    break;
                }

                addConnection(graph, src, dest);
                printf("Connection Added: %s <-> %s\n", source, destination);

                break;
            }

            case 4:
            {
                promptStationName("Source Station", source, sizeof(source));
                promptStationName("Destination Station", destination, sizeof(destination));

                int src = findStation(graph, source);
                int dest = findStation(graph, destination);

                if((src == -1) || (dest == -1))
                {
                    printf("\nInvalid Station\n");
                    break;
                }

                removeConnection(graph, src, dest);

                break;
            }

            case 5:
            {
                promptStationName("Source", source, sizeof(source));
                promptStationName("Destination", destination, sizeof(destination));

                int src = findStation(graph, source);
                int dest = findStation(graph, destination);

                if((src == -1) || (dest == -1))
                {
                    printf("\nInvalid Station\n");
                    break;
                }

                BFS(graph, src, dest);

                break;
            }

            case 6:

                (void)saveNetwork(graph, NETWORK_FILE);
                break;

            case 7:

                (void)loadNetwork(graph, NETWORK_FILE);
                break;

            case 8:

                return;

            default:

                /* readMenuChoice() already guarantees the
                   value is in [1,8], so this branch is
                   unreachable - kept only as a defensive
                   MISRA-style default case. */
                printf("\nInvalid Choice\n");
                logWarning("Invalid Admin Menu Choice");
        }
    }
}
