#ifndef THREADS_TIETO_ASSIGNMENT
#define THREADS_TIETO_ASSIGNMENT

#include "cpu.h"
#include "pthread.h"

#define THREADS_NUM 3

typedef struct {
	int *pipefd;
	pthread_mutex_t *analyzerPrinterMutex;
	pthread_mutex_t *watchdogMutex;
	pthread_cond_t *condvar;
	float *cpus_usage;
	CPUStats *cpus;
	short *alive;
	pthread_t *threads;
} ThreadsArgs;

void *Reader(void *);
void *Analyzer(void *);
void *Printer(void *);
void *Watchdog(void *);

#endif	// THREADS_TIETO_ASSIGNMENT
