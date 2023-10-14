#include "Threads.h"
#include "cpu.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void CheckAlive(pthread_mutex_t *watchdogMutex, short *alive, short i)
{
	// Mark that thread is alive, so watchdog doesnt kill whole app
	pthread_mutex_lock(watchdogMutex);
	alive[i] = 1;
	pthread_mutex_unlock(watchdogMutex);
}

void *Reader(void *arg)
{
	// Unpack arg
	ThreadsArgs *args = (ThreadsArgs *) arg;
	CPUStats cpu;
	char buffer[128];

	// Read data once a second
	while(1) {
		CheckAlive(args->watchdogMutex, args->alive, 0);

		// Read file
		FILE *file = fopen("/proc/stat", "r");
		if(!file) {
			perror("Can't read /proc/stat!");
			exit(2);
		}
		// Read to buffer size or to new line
		while(fgets(buffer, sizeof(buffer), file)) {
			// If starts with 'cpuXX', its about one of the cores, but we need
			// to omit 'cpu' line
			if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4)) {
				// Parse data
				sscanf(buffer, "cpu%d %d %d %d %d %d %d %d %d %d %d", &cpu.id,
					   &cpu.user, &cpu.nice, &cpu.system, &cpu.idle,
					   &cpu.iowait, &cpu.irq, &cpu.softirq, &cpu.steal,
					   &cpu.guest, &cpu.guest_nice);
				// Send them through pipe
				write(args->pipefd[1], &cpu, sizeof(cpu));
			}
		}
		// Close /proc/stat
		fclose(file);
		// write to watchdog
		sleep(1);
	}

	return NULL;
}

void *Analyzer(void *arg)
{
	// Unpack args and create necessary var
	ThreadsArgs *args = (ThreadsArgs *) arg;
	int cc			  = GetCoreCount();
	// buffer size is core count plus 1(to calc usage we need last measurment
	// from same core)
	int bufferSize	  = cc + 1;
	int n;
	float result;
	int index = 0;
	// First time fill bufor
	for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
		// Read from pipe
		n = read(args->pipefd[0], &args->cpus[index], sizeof(CPUStats));
		if(n <= 0)
			return NULL;
	}
	// continuously read from pipe
	while(1) {
		CheckAlive(args->watchdogMutex, args->alive, 1);

		for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
			n = read(args->pipefd[0], &args->cpus[index], sizeof(CPUStats));
			if(n <= 0)	// if pipe closes
				break;

			// calc usage with last record for same core
			result = CalculateCpuUsage(args->cpus[(index + 1) % bufferSize],
									   args->cpus[index]);
			// write usage into shared variable
			pthread_mutex_lock(args->analyzerPrinterMutex);
			args->cpus_usage[i] = result;
			pthread_mutex_unlock(args->analyzerPrinterMutex);
		}
		// After whole set notify Printer
		pthread_cond_signal(args->condvar);
	}

	pthread_exit(NULL);
}

void *Printer(void *arg)
{
	// Unpack args
	ThreadsArgs *args = (ThreadsArgs *) arg;
	int cc			  = GetCoreCount();
	while(1) {
		CheckAlive(args->watchdogMutex, args->alive, 2);
		// Before cond_wait, mutex must be locked
		pthread_mutex_lock(args->analyzerPrinterMutex);
		// Wait for signal
		pthread_cond_wait(args->condvar, args->analyzerPrinterMutex);

		printf("CPU USAGE:\n");
		// Nicely print CPU usage
		for(int i = 0; i < cc; i++)
			printf("Cpu%i: %0.1f%%\n", i, args->cpus_usage[i]);

		// Free mutex
		pthread_mutex_unlock(args->analyzerPrinterMutex);
		printf("\n");
	}

	pthread_exit(NULL);
}

void *Watchdog(void *arg)
{
	// Unpack args
	ThreadsArgs *args = (ThreadsArgs *) arg;

	while(1) {
		sleep(2);
		// Check if threads marked their existence
		for(int i = 0; i < THREADS_NUM; i++) {
			pthread_mutex_lock(args->watchdogMutex);
			if(!args->alive[i]) {
				pthread_mutex_unlock(args->watchdogMutex);
				// Someone is dead, execute order 66
				printf("Killing app!\n");
				for(int i = 0; i < THREADS_NUM; i++)
					pthread_cancel(args->threads[i]);

				pthread_exit(NULL);
			}
			else  // Trust, but check
				args->alive[i] = 0;
			pthread_mutex_unlock(args->watchdogMutex);
		}
	}

	pthread_exit(NULL);
}