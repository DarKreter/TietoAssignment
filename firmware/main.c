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
	pthread_mutex_t mutex;
	pthread_cond_t condvar;

	// Config and init threads, attr, mutex and condvar
	pthread_t reader, analyzer, printer;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condvar, NULL);

	// Create pipe between Reader and Analyzer
	int pipeReaderAnalyzer[2];
	pipe(pipeReaderAnalyzer);

	// Pack args for threads
	AnalyzerArgs analyzerArgs = {pipeReaderAnalyzer, &mutex, &condvar,
								 cpus_usage};
	pthread_create(&reader, &attr, Reader, (void *) pipeReaderAnalyzer);
	pthread_create(&analyzer, &attr, Analyzer, (void *) &analyzerArgs);
	pthread_create(&printer, &attr, Printer, (void *) &analyzerArgs);

	// Wait for threads
	pthread_join(reader, NULL);
	pthread_join(analyzer, NULL);
	pthread_join(printer, NULL);

	// Clean up
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condvar);

	return 0;
}

// TODO:
// SIGTERM handler
// Add make tests
// Test with valgrind
// Add watchdog
// Add logger