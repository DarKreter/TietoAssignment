#ifndef UTILS_TIETO_ASSIGNMENT
#define UTILS_TIETO_ASSIGNMENT

#include "Threads.h"

int CheckAlive(pthread_mutex_t *watchdogMutex, short *alive, short i);
void Log(const char *mess, ThreadsArgs *args);
void LogInt(int mess, ThreadsArgs *args);

#endif	// UTILS_TIETO_ASSIGNMENT
