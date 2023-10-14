#ifndef THREADS_TIETO_ASSIGNMENT
#define THREADS_TIETO_ASSIGNMENT

#include "pthread.h"

typedef struct {
	int *pipefd;
	pthread_mutex_t *mutex;
	pthread_cond_t *condvar;
	float *cpus_usage;
} AnalyzerArgs;

void *Reader(void *);
void *Analyzer(void *);
void *Printer();

#endif	// THREADS_TIETO_ASSIGNMENT