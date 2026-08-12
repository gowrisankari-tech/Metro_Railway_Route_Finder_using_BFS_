#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <stdint.h>

/*
 * Starts workerCount POSIX threads. Each worker performs
 * iterations increments on one shared counter protected by a
 * mutex. The final count is returned through result.
 *
 * Returns 1 on success and 0 on invalid arguments or a thread
 * creation/join failure.
 */
int mtRunWorkers(uint32_t workerCount,
                 uint32_t iterations,
                 uint64_t *result);

#endif
