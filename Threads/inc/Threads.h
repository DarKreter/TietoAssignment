#ifndef THREADS_TIETO_ASSIGNMENT
#define THREADS_TIETO_ASSIGNMENT

#include "cpu.h"
#include "pthread.h"
#include <stdio.h>

#define THREADS_NUM 4

extern int terminate;

typedef struct {
	int *pipeReadAnal;
	int *pipeLogger;
	pthread_mutex_t *analyzerPrinterMutex;
	pthread_mutex_t *watchdogMutex;
	pthread_mutex_t *loggerMutex;
	pthread_cond_t *analyzerPrinterCondvar;
	float *cpus_usage;
	CPUStats *cpus;
	short *alive;
	pthread_t *threads;
	FILE *loggerFile;
} ThreadsArgs;

void *Reader(void *);
void *Analyzer(void *);
void *Printer(void *);
void *Watchdog(void *);
void *Logger(void *);

#endif	// THREADS_TIETO_ASSIGNMENT
