#include <stdio.h>
#include <time.h>

#include "logger.h"

static void writeLog(const char *level, const char *msg)
{
    FILE *fp = fopen("app.log", "a");

    if(fp == NULL)
    {
        return;
    }

    time_t now = time(NULL);
    char timestamp[32];

    if(now == (time_t)(-1))
    {
        /* time() itself can fail on some platforms; fall back
           to a placeholder rather than skip the log line. */
        (void)snprintf(timestamp, sizeof(timestamp), "UNKNOWN-TIME");
    }
    else
    {
        struct tm *localTime = localtime(&now);

        if(localTime == NULL)
        {
            (void)snprintf(timestamp, sizeof(timestamp), "UNKNOWN-TIME");
        }
        else if(strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime) == 0U)
        {
            (void)snprintf(timestamp, sizeof(timestamp), "UNKNOWN-TIME");
        }
    }

    fprintf(fp, "[%s] [%s] %s\n", timestamp, level, msg);

    fclose(fp);
}

void logInfo(const char *msg)
{
    writeLog("INFO", msg);
}

void logWarning(const char *msg)
{
    writeLog("WARNING", msg);
}

void logError(const char *msg)
{
    writeLog("ERROR", msg);
}
