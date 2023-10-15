#include "Threads.h"
#include "cpu.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int terminate = 0;

void *Reader(void *arg)
{
	// Unpack arg
	ThreadsArgs *args = (ThreadsArgs *) arg;
	CPUStats cpu;
	char buffer[128];
	FILE *file;
	// Read data once a second
	while(1) {
		if(CheckAlive(args->watchdogMutex, args->alive, 0)) {
			close(args->pipeReadAnal[1]);
			pthread_exit(NULL);
		}

		// Read file
		file = fopen("/proc/stat", "r");
		if(!file) {
			Log("Can't read /proc/stat!\n", args);
			pthread_exit(NULL);
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
				write(args->pipeReadAnal[1], &cpu, sizeof(cpu));
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
	long n;
	float result;
	int index = 0;
	// First time fill bufor
	for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
		// Read from pipe
		n = read(args->pipeReadAnal[0], &args->cpus[index], sizeof(CPUStats));
		if(n <= 0)
			return NULL;
	}
	// continuously read from pipe
	while(1) {
		if(CheckAlive(args->watchdogMutex, args->alive, 1)) {
			close(args->pipeReadAnal[0]);
			pthread_exit(NULL);
		}

		for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
			n = read(args->pipeReadAnal[0], &args->cpus[index],
					 sizeof(CPUStats));
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
		pthread_cond_signal(args->analyzerPrinterCondvar);
	}

	// pthread_exit(NULL);
}

void *Printer(void *arg)
{
	// Unpack args
	ThreadsArgs *args = (ThreadsArgs *) arg;
	int cc			  = GetCoreCount();
	while(1) {
		if(CheckAlive(args->watchdogMutex, args->alive, 2))
			pthread_exit(NULL);
		// Before cond_wait, mutex must be locked
		pthread_mutex_lock(args->analyzerPrinterMutex);
		// Wait for signal
		pthread_cond_wait(args->analyzerPrinterCondvar,
						  args->analyzerPrinterMutex);

		printf("CPU USAGE:\n");
		// Nicely print CPU usage
		for(int i = 0; i < cc; i++)
			printf("Cpu%i: %0.1f%%\n", i, (double) args->cpus_usage[i]);

		// Free mutex
		pthread_mutex_unlock(args->analyzerPrinterMutex);
		printf("\n");
	}

	// pthread_exit(NULL);
}



void *Watchdog(void *arg)
{
	// Unpack args
	ThreadsArgs *args = (ThreadsArgs *) arg;

	while(1) {
		sleep(2);
		if(terminate == 1) {
			Log("Got SIGTERM, terminating app!", args);
			sleep(1);
			KillApp(args);

			pthread_exit(NULL);
		}
		// Check if threads marked their existence
		for(int i = 0; i < THREADS_NUM - 1; i++) {
			pthread_mutex_lock(args->watchdogMutex);
			if(!args->alive[i]) {
				pthread_mutex_unlock(args->watchdogMutex);

				// Log inf
				Log("Watchdog: Killing app, because of inactive thread: ",
					args);
				LogInt(i, args);
				Log("!\n", args);
				sleep(1);
				KillApp(args);

				pthread_exit(NULL);
			}
			else  // Trust, but check
				args->alive[i] = 0;
			pthread_mutex_unlock(args->watchdogMutex);
		}
	}

	// pthread_exit(NULL);
}

void *Logger(void *arg)
{
	// Unpack args
	ThreadsArgs *args = (ThreadsArgs *) arg;
	char buff;
	int n;
	if(!args->loggerFile) {
		perror("Cant open data.log!\n");
		pthread_exit(NULL);
	}
	while(1) {
		if(CheckAlive(args->watchdogMutex, args->alive, 3)) {
			close(args->pipeLogger[0]);
			pthread_exit(NULL);
		}
		n = read(args->pipeLogger[0], &buff, sizeof(char));
		if(n <= 0)	// if pipe closes
			continue;

		fputc(buff, args->loggerFile);
	}

	// pthread_exit(NULL);
}