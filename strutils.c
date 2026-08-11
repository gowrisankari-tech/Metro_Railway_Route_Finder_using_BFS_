#include <ctype.h>
#include <stddef.h>

#include "strutils.h"

int stringsEqualIgnoreCase(const char *a, const char *b)
{
    if((a == NULL) || (b == NULL))
    {
        return 0;
    }

    while((*a != '\0') && (*b != '\0'))
    {
        unsigned char ca = (unsigned char)*a;
        unsigned char cb = (unsigned char)*b;

        if(tolower(ca) != tolower(cb))
        {
            return 0;
        }

        a++;
        b++;
    }

    return ((*a == '\0') && (*b == '\0')) ? 1 : 0;
}
