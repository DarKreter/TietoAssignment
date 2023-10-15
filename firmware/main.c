#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "Threads.h"
#include "utils.h"

static void RunTests(void)
{
	assert(2 + 2 == 4);

	CPUStats prev, curr;
	prev.idle		= 5000;
	prev.iowait		= 0;
	prev.user		= 2500;
	prev.nice		= 0;
	prev.system		= 2500;
	prev.irq		= 0;
	prev.softirq	= 0;
	prev.steal		= 0;
	prev.guest		= 0;
	prev.guest_nice = 0;
	curr.idle		= 10000;
	curr.iowait		= 0;
	curr.user		= 5000;
	curr.nice		= 0;
	curr.system		= 5000;
	curr.irq		= 0;
	curr.softirq	= 0;
	curr.steal		= 0;
	curr.guest		= 0;
	curr.guest_nice = 0;

	assert(50.f == CalculateCpuUsage(prev, curr));
}

int main()
{
	// Run tests
	RunTests();

	// shared data between Analyzer and Printer
	float *cpus_usage = malloc(sizeof(float) * (unsigned long) GetCoreCount());
	pthread_mutex_t analyzerPrinterMutex, watchdogMutex, loggerMutex;
	pthread_cond_t analyzerPrinterCondvar;
	// Analyzer data
	CPUStats *cpus =
		malloc(sizeof(CPUStats) * (1ul + (unsigned long) GetCoreCount()));
	// Watchdog table
	short *alive = malloc(sizeof(short) * THREADS_NUM);
	alive[0] = alive[1] = alive[2] = alive[3] = 1;
	// Logger file
	FILE *lgfl								  = fopen("data.log", "w+");

	// Config and init threads, attr, mutex and condvar
	pthread_t threads[THREADS_NUM], watchdog;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&analyzerPrinterMutex, NULL);
	pthread_mutex_init(&watchdogMutex, NULL);
	pthread_mutex_init(&loggerMutex, NULL);
	pthread_cond_init(&analyzerPrinterCondvar, NULL);

	// Create pipes
	int pipeReaderAnalyzer[2], pipeLogger[2];
	pipe(pipeReaderAnalyzer);
	pipe(pipeLogger);


	// Pack args for threads
	ThreadsArgs args = {pipeReaderAnalyzer,
						pipeLogger,
						&analyzerPrinterMutex,
						&watchdogMutex,
						&loggerMutex,
						&analyzerPrinterCondvar,
						cpus_usage,
						cpus,
						alive,
						threads,
						lgfl};
	pthread_create(&threads[0], &attr, Reader, (void *) &args);
	pthread_create(&threads[1], &attr, Analyzer, (void *) &args);
	pthread_create(&threads[2], &attr, Printer, (void *) &args);
	pthread_create(&threads[3], &attr, Logger, (void *) &args);
	pthread_create(&watchdog, &attr, Watchdog, (void *) &args);

	// Capture SIGTERM
	signal(SIGTERM, Capture);

	// Wait for threads
	for(int i = 0; i < THREADS_NUM; i++)
		pthread_join(threads[i], NULL);
	pthread_join(watchdog, NULL);

	// Clean up
	fclose(lgfl);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&analyzerPrinterMutex);
	pthread_mutex_destroy(&watchdogMutex);
	pthread_mutex_destroy(&loggerMutex);
	pthread_cond_destroy(&analyzerPrinterCondvar);
	free(cpus_usage);
	free(cpus);
	free(alive);

	return 0;
}

// TODO:
// Test with gcc and clang
// Test with valgrind
// Add README
