#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "validation.h"
#include "../graph/graph.h"
#include "../logger/logger.h"

uint8_t validateMenuChoice(int choice, int minChoice, int maxChoice)
{
    if((choice >= minChoice) && (choice <= maxChoice))
    {
        return 1U;
    }

    logWarning("Invalid Menu Choice");

    return 0U;
}

uint8_t validateStationName(const char *name)
{
    if(name == NULL)
    {
        logError("NULL Station Name");
        return 0U;
    }

    if(strlen(name) == 0U)
    {
        logWarning("Empty Station Name");
        return 0U;
    }

    for(size_t i = 0U; i < strlen(name); i++)
    {
        if((isalnum((unsigned char)name[i]) == 0) && (name[i] != ' '))
        {
            logWarning("Invalid Characters In Station Name");
            return 0U;
        }
    }

    return 1U;
}

uint8_t validateStationIndex(int index)
{
    if((index >= 0) && (index < (int)MAX_STATIONS))
    {
        return 1U;
    }

    logWarning("Invalid Station Index");

    return 0U;
}
