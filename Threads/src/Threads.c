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
	int *pipefd	 = (int(*)) _pipe;
	int cc		 = CheckCoreCount();
	short parity = 1;
	//, first = 1;
	int n;
	float result;
	CPUStats *cpus_1 = malloc(sizeof(CPUStats) * cc);
	CPUStats *cpus_2 = malloc(sizeof(CPUStats) * cc);
	CPUStats *curr = cpus_1, *prev = cpus_2;
	// First time fill bufor
	for(int i = 0; i < cc; i++) {
		n = read(pipefd[0], &curr[i], sizeof(CPUStats));
		if(n <= 0)
			return NULL;
	}

	while(1) {
		if(parity) {
			curr   = cpus_2;
			prev   = cpus_1;
			parity = 0;
		}
		else {
			curr   = cpus_1;
			prev   = cpus_2;
			parity = 1;
		}
		for(int i = 0; i < cc; i++) {
			n = read(pipefd[0], &curr[i], sizeof(CPUStats));
			if(n <= 0)
				break;


			result = CalculateCpuUsage(prev[i], curr[i]);
			printf("cpu%d: %f%%\n", curr[i].id, result);
		}
		printf("\n");
	}

	return NULL;
}