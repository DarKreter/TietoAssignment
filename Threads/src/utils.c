#include "utils.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

void Capture(int sig)
{
	if(sig == SIGTERM)
		terminate = 1;
}

int CheckAlive(pthread_mutex_t *watchdogMutex, short *alive, short i)
{
	// Mark that thread is alive, so watchdog doesnt kill whole app
	pthread_mutex_lock(watchdogMutex);
	if(alive[i] == -1) {
		alive[i] = -2;
		pthread_mutex_unlock(watchdogMutex);
		return 1;
	}
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

void KillApp(ThreadsArgs *args)
{
	// Try to kill threads gracefully
	pthread_mutex_lock(args->watchdogMutex);
	for(int j = 0; j < THREADS_NUM; j++)
		args->alive[j] = -1;
	pthread_mutex_unlock(args->watchdogMutex);
	pthread_cond_signal(args->analyzerPrinterCondvar);
	close(args->pipeLogger[1]);
	sleep(2);
	for(int j = 0; j < THREADS_NUM; j++)
		if(args->alive[j] != -2)  // Force kill
		{						  // Someone is dead, execute order 66

			Log("Can't kill threads gracefully, force killing (", args);
			LogInt(j, args);
			Log(")!\n", args);
			for(int j = 0; j < THREADS_NUM; j++)
				pthread_cancel(args->threads[j]);
			break;
		}
}