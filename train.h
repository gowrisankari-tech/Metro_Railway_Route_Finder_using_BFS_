#ifndef TRAIN_H
#define TRAIN_H

#define TRAIN_NAME_LEN 50

typedef struct
{
    char source[50];
    char destination[50];
    char trainName[TRAIN_NAME_LEN];
    char runningDays[50];
    int travelTime;
} TrainInfo;

void showAvailableTrains(const char *source, const char *destination);

#endif
