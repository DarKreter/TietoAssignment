#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float CalculateCpuUsage(CPUStats prev, CPUStats curr)
{
	int PrevIdle = prev.idle + prev.iowait;
	int Idle	 = curr.idle + curr.iowait;

	int PrevNonIdle = prev.user + prev.nice + prev.system + prev.irq +
					  prev.softirq + prev.steal;
	int NonIdle = curr.user + curr.nice + curr.system + curr.irq +
				  curr.softirq + curr.steal;

	int PrevTotal = PrevIdle + PrevNonIdle;
	int Total	  = Idle + NonIdle;

	int totald = Total - PrevTotal;
	int idled  = Idle - PrevIdle;

	return 100 * (totald - idled) / totald;
}

short GetCoreCount()
{
	FILE *file = fopen("/proc/cpuinfo", "r");
	if(!file) {
		perror("Failed to open /proc/cpuinfo");
		exit(1);
	}
	// Count number of cpu cores
	int cores = 0;
	char line[128];
	while(fgets(line, sizeof(line), file))
		if(!strncmp(line, "processor", 9))
			cores++;

	fclose(file);

	return cores;
}
