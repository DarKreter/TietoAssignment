#include "utils.h"
#include <string.h>
#include <unistd.h>

int CheckAlive(pthread_mutex_t *watchdogMutex, short *alive, short i)
{
	// Mark that thread is alive, so watchdog doesnt kill whole app
	pthread_mutex_lock(watchdogMutex);
	if(alive[i] == -1) {
		alive[i] = -2;
		pthread_mutex_unlock(watchdogMutex);
		return 1;
	}
	if(i != 2)
		alive[i] = 1;
	pthread_mutex_unlock(watchdogMutex);
	return 0;
}

void Log(const char *mess, ThreadsArgs *args)
{
	pthread_mutex_lock(args->loggerMutex);
	write(args->pipeLogger[1], mess, strlen(mess));
	pthread_mutex_unlock(args->loggerMutex);
}

void LogInt(int mess, ThreadsArgs *args)
{
	char str[20];
	pthread_mutex_lock(args->loggerMutex);
	sprintf(str, "%d", mess);
	write(args->pipeLogger[1], str, strlen(str));
	pthread_mutex_unlock(args->loggerMutex);
}