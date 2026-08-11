#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "../validation/validation.h"
#include "../logger/logger.h"

void clearInputBuffer(void)
{
    int ch;

    while(((ch = getchar()) != '\n') && (ch != EOF))
    {
        /* Empty */
    }
}

int readMenuChoice(int minChoice, int maxChoice)
{
    int choice = 0;

    while(1)
    {
        printf("\nEnter Choice : ");

        int result = scanf("%d", &choice);

        if(result == EOF)
        {
            /* Input stream closed (e.g. piped input ran out,
               or Ctrl+D). Looping forever here would spin the
               CPU re-reading EOF endlessly - exit cleanly
               instead. */
            printf("\nInput stream closed. Exiting.\n");
            logWarning("readMenuChoice: EOF on stdin, exiting");
            exit(0);
        }

        if(result != 1)
        {
            /* Non-numeric input: scanf leaves choice untouched
               and the bad characters still sit in stdin. Clear
               it before re-prompting so we don't loop forever
               re-reading the same bad token. */
            clearInputBuffer();
            printf("Invalid input - please enter a number.\n");
            continue;
        }

        clearInputBuffer();

        if(validateMenuChoice(choice, minChoice, maxChoice) == 1U)
        {
            return choice;
        }

        printf("Choice must be between %d and %d.\n", minChoice, maxChoice);
    }
}

void readLineInput(char *buffer, size_t size)
{
    if((buffer == NULL) || (size == 0U))
    {
        return;
    }

    if(fgets(buffer, (int)size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}

void showAdminMenu(void)
{
    printf("\n");
    printf("=====================================\n");
    printf(" ADMIN MENU\n");
    printf("=====================================\n");
    printf("1. View Stations\n");
    printf("2. Display Network\n");
    printf("3. Add Connection\n");
    printf("4. Remove Connection\n");
    printf("5. Find Shortest Route\n");
    printf("6. Save Network\n");
    printf("7. Load Network\n");
    printf("8. Back To Main Menu\n");
    printf("=====================================\n");
}

void initializeDefaultNetwork(Graph *graph)
{
    /* =========================
       GREEN LINE METRO
       ========================= */

    int centralMetro      = addStation(graph, "Central Metro", NETWORK_METRO);
    int egmoreMetro       = addStation(graph, "Egmore Metro", NETWORK_METRO);
    int nehruPark         = addStation(graph, "Nehru Park", NETWORK_METRO);
    int kilpauk           = addStation(graph, "Kilpauk", NETWORK_METRO);
    int shenoyNagar       = addStation(graph, "Shenoy Nagar", NETWORK_METRO);
    int annaNagarEast     = addStation(graph, "Anna Nagar East", NETWORK_METRO);
    int koyambedu         = addStation(graph, "Koyambedu", NETWORK_METRO);
    int vadapalani        = addStation(graph, "Vadapalani", NETWORK_METRO);
    int ashokNagar        = addStation(graph, "Ashok Nagar", NETWORK_METRO);
    int alandurMetro      = addStation(graph, "Alandur Metro", NETWORK_METRO);

    /* =========================
       BLUE LINE METRO
       ========================= */

    int govtEstate        = addStation(graph, "Government Estate", NETWORK_METRO);
    int lic               = addStation(graph, "LIC", NETWORK_METRO);
    int thousandLights    = addStation(graph, "Thousand Lights", NETWORK_METRO);
    int agDms             = addStation(graph, "AG-DMS", NETWORK_METRO);
    int teynampet          = addStation(graph, "Teynampet", NETWORK_METRO);
    int nandanam           = addStation(graph, "Nandanam", NETWORK_METRO);
    int saidapet           = addStation(graph, "Saidapet", NETWORK_METRO);
    int littleMount         = addStation(graph, "Little Mount", NETWORK_METRO);
    int guindyMetro         = addStation(graph, "Guindy Metro", NETWORK_METRO);
    int airport             = addStation(graph, "Airport", NETWORK_METRO);

    /* =========================
       RAILWAY NETWORK
       ========================= */

    int chennai         = addStation(graph, "Chennai", NETWORK_RAILWAY);
    int bengaluru       = addStation(graph, "Bengaluru", NETWORK_RAILWAY);
    int mysuru          = addStation(graph, "Mysuru", NETWORK_RAILWAY);
    int hyderabad       = addStation(graph, "Hyderabad", NETWORK_RAILWAY);
    int vijayawada      = addStation(graph, "Vijayawada", NETWORK_RAILWAY);
    int visakhapatnam   = addStation(graph, "Visakhapatnam", NETWORK_RAILWAY);
    int nagpur          = addStation(graph, "Nagpur", NETWORK_RAILWAY);
    int bhopal          = addStation(graph, "Bhopal", NETWORK_RAILWAY);
    int indore          = addStation(graph, "Indore", NETWORK_RAILWAY);
    int pune            = addStation(graph, "Pune", NETWORK_RAILWAY);
    int mumbai          = addStation(graph, "Mumbai", NETWORK_RAILWAY);
    int ahmedabad       = addStation(graph, "Ahmedabad", NETWORK_RAILWAY);
    int jaipur          = addStation(graph, "Jaipur", NETWORK_RAILWAY);
    int delhi           = addStation(graph, "Delhi", NETWORK_RAILWAY);
    int lucknow         = addStation(graph, "Lucknow", NETWORK_RAILWAY);
    int kanpur          = addStation(graph, "Kanpur", NETWORK_RAILWAY);
    int patna           = addStation(graph, "Patna", NETWORK_RAILWAY);
    int kolkata         = addStation(graph, "Kolkata", NETWORK_RAILWAY);
    int bhubaneswar     = addStation(graph, "Bhubaneswar", NETWORK_RAILWAY);
    int coimbatore      = addStation(graph, "Coimbatore", NETWORK_RAILWAY);

    /* =========================
       GREEN LINE CONNECTIONS
       ========================= */

    addConnection(graph, centralMetro, egmoreMetro);
    addConnection(graph, egmoreMetro, nehruPark);
    addConnection(graph, nehruPark, kilpauk);
    addConnection(graph, kilpauk, shenoyNagar);
    addConnection(graph, shenoyNagar, annaNagarEast);
    addConnection(graph, annaNagarEast, koyambedu);
    addConnection(graph, koyambedu, vadapalani);
    addConnection(graph, vadapalani, ashokNagar);
    addConnection(graph, ashokNagar, alandurMetro);

    /* =========================
       BLUE LINE CONNECTIONS
       ========================= */

    addConnection(graph, centralMetro, govtEstate);
    addConnection(graph, govtEstate, lic);
    addConnection(graph, lic, thousandLights);
    addConnection(graph, thousandLights, agDms);
    addConnection(graph, agDms, teynampet);
    addConnection(graph, teynampet, nandanam);
    addConnection(graph, nandanam, saidapet);
    addConnection(graph, saidapet, littleMount);
    addConnection(graph, littleMount, guindyMetro);
    addConnection(graph, guindyMetro, alandurMetro);
    addConnection(graph, alandurMetro, airport);

    /* =========================
       RAILWAY LINE CONNECTIONS
       ========================= */

    addConnection(graph, chennai, vijayawada);
    addConnection(graph, vijayawada, nagpur);
    addConnection(graph, nagpur, delhi);

    addConnection(graph, chennai, bengaluru);
    addConnection(graph, bengaluru, pune);
    addConnection(graph, pune, mumbai);
    addConnection(graph, mumbai, delhi);

    addConnection(graph, chennai, hyderabad);
    addConnection(graph, hyderabad, bhopal);
    addConnection(graph, bhopal, jaipur);
    addConnection(graph, jaipur, delhi);

    addConnection(graph, delhi, lucknow);
    addConnection(graph, lucknow, kanpur);
    addConnection(graph, kanpur, patna);
    addConnection(graph, patna, kolkata);

    addConnection(graph, kolkata, bhubaneswar);
    addConnection(graph, bhubaneswar, visakhapatnam);
    addConnection(graph, visakhapatnam, vijayawada);

    addConnection(graph, pune, ahmedabad);
    addConnection(graph, ahmedabad, jaipur);

    addConnection(graph, hyderabad, nagpur);
    addConnection(graph, indore, bhopal);
    addConnection(graph, indore, nagpur);

    addConnection(graph, chennai, coimbatore);
    addConnection(graph, coimbatore, bengaluru);
    addConnection(graph, bengaluru, mysuru);

    /* =========================
       INTERCHANGE
       ========================= */

    addConnection(graph, centralMetro, chennai);
}
