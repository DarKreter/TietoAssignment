#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "Threads.h"

int main()
{
	// shared data between Analyzer and Printer
	float *cpus_usage = malloc(sizeof(float) * GetCoreCount());
	pthread_mutex_t analyzerPrinterMutex, watchdogMutex;
	pthread_cond_t analyzerPrinterCondvar;
	// Analyzer data
	CPUStats *cpus = malloc(sizeof(CPUStats) * (1 + GetCoreCount()));
	// Watchdog table
	short *alive   = malloc(sizeof(short) * THREADS_NUM);
	alive[0] = alive[1] = alive[2] = alive[3] = 1;

	// Config and init threads, attr, mutex and condvar
	pthread_t threads[THREADS_NUM], watchdog;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&analyzerPrinterMutex, NULL);
	pthread_mutex_init(&watchdogMutex, NULL);
	pthread_cond_init(&analyzerPrinterCondvar, NULL);

	// Create pipe between Reader and Analyzer
	int pipeReaderAnalyzer[2];
	pipe(pipeReaderAnalyzer);

	// Pack args for threads
	ThreadsArgs analyzerArgs = {pipeReaderAnalyzer,
								&analyzerPrinterMutex,
								&watchdogMutex,
								&analyzerPrinterCondvar,
								cpus_usage,
								cpus,
								alive,
								threads};
	pthread_create(&threads[0], &attr, Reader, (void *) &analyzerArgs);
	pthread_create(&threads[1], &attr, Analyzer, (void *) &analyzerArgs);
	pthread_create(&threads[2], &attr, Printer, (void *) &analyzerArgs);
	pthread_create(&watchdog, &attr, Watchdog, (void *) &analyzerArgs);

	// Wait for threads
	for(int i = 0; i < THREADS_NUM; i++)
		pthread_join(threads[i], NULL);
	pthread_join(watchdog, NULL);

	// Clean up
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&analyzerPrinterMutex);
	pthread_mutex_destroy(&watchdogMutex);
	pthread_cond_destroy(&analyzerPrinterCondvar);
	free(cpus_usage);
	free(cpus);
	free(alive);

	return 0;
}

// TODO:
// SIGTERM handler
// Add make tests
// Test with valgrind
// Add logger