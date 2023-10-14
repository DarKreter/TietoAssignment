#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *Reader(void *_pipe)
{
	int *pipefd = (int *) (_pipe);
	CPUStats cpu;
	char buffer[128];

	while(1) {
		FILE *file = fopen("/proc/stat", "r");
		if(!file) {
			perror("Can't read /proc/stat!");
			exit(2);
		}
		while(fgets(buffer, sizeof(buffer), file)) {
			if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4)) {
				sscanf(buffer, "cpu%hd %d %d %d %d %d %d %d %d %d %d", &cpu.id,
					   &cpu.user, &cpu.nice, &cpu.system, &cpu.idle,
					   &cpu.iowait, &cpu.irq, &cpu.softirq, &cpu.steal,
					   &cpu.guest, &cpu.guest_nice);
				write(pipefd[1], &cpu, sizeof(cpu));
			}
		}
		fclose(file);

		sleep(1);
	}

	return NULL;
}

void *Analyzer(void *_pipe)
{
	int *pipefd	   = (int(*)) _pipe;
	int cc		   = CheckCoreCount();
	int bufferSize = cc + 1;
	int n;
	float result;
	CPUStats *cpus = malloc(sizeof(CPUStats) * bufferSize);
	int index	   = 0;
	// First time fill bufor
	for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
		n = read(pipefd[0], &cpus[index], sizeof(CPUStats));
		if(n <= 0) {
			free(cpus);
			return NULL;
		}
	}
	while(1) {
		for(int i = 0; i < cc; i++, index = (index + 1) % bufferSize) {
			n = read(pipefd[0], &cpus[index], sizeof(CPUStats));
			if(n <= 0)
				break;

			result =
				CalculateCpuUsage(cpus[(index + 1) % bufferSize], cpus[index]);
			printf("cpu%d: %f%%\n", cpus[i].id, result);
		}
		printf("\n");
	}

	free(cpus);
	return NULL;
}