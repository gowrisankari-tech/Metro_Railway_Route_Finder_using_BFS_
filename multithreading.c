/*
 * multithreading.c
 *
 * Small, isolated POSIX-thread module for the project.
 * It demonstrates:
 *   - pthread_create()
 *   - pthread_join()
 *   - pthread_mutex_lock()/unlock()
 *   - shared-data protection
 *
 * The module is intentionally separate from the route-finding
 * logic so the existing single-threaded menu behavior is not
 * changed.
 */

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "multithreading.h"

typedef struct
{
    uint32_t iterations;
    uint64_t *counter;
    pthread_mutex_t *mutex;
} WorkerArgs;

static void *workerFunction(void *arg)
{
    WorkerArgs *args = (WorkerArgs *)arg;

    if((args == NULL) || (args->counter == NULL) || (args->mutex == NULL))
    {
        return NULL;
    }

    for(uint32_t i = 0U; i < args->iterations; i++)
    {
        if(pthread_mutex_lock(args->mutex) != 0)
        {
            return NULL;
        }

        (*args->counter)++;

        (void)pthread_mutex_unlock(args->mutex);
    }

    return NULL;
}

int mtRunWorkers(uint32_t workerCount,
                 uint32_t iterations,
                 uint64_t *result)
{
    if((workerCount == 0U) || (iterations == 0U) || (result == NULL))
    {
        return 0;
    }

    pthread_t *threads = (pthread_t *)calloc(workerCount, sizeof(pthread_t));
    if(threads == NULL)
    {
        return 0;
    }

    uint64_t counter = 0U;
    pthread_mutex_t mutex;

    if(pthread_mutex_init(&mutex, NULL) != 0)
    {
        free(threads);
        return 0;
    }

    WorkerArgs args;
    args.iterations = iterations;
    args.counter = &counter;
    args.mutex = &mutex;

    uint32_t created = 0U;
    int success = 1;

    for(uint32_t i = 0U; i < workerCount; i++)
    {
        if(pthread_create(&threads[i], NULL, workerFunction, &args) != 0)
        {
            success = 0;
            break;
        }

        created++;
    }

    for(uint32_t i = 0U; i < created; i++)
    {
        if(pthread_join(threads[i], NULL) != 0)
        {
            success = 0;
        }
    }

    (void)pthread_mutex_destroy(&mutex);
    free(threads);

    *result = counter;

    return success;
}
