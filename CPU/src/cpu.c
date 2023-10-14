#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"


float CalculateCpuUsage(CPUStats curr)
{
    static CPUStats prev = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int PrevIdle = prev.idle + prev.iowait;
    int Idle = curr.idle + curr.iowait;
    printf("%d %d\n", PrevIdle, Idle);

    int PrevNonIdle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    int NonIdle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;
    printf("%d %d\n", PrevNonIdle, NonIdle);

    int PrevTotal = PrevIdle + PrevNonIdle;
    int Total = Idle + NonIdle;
    printf("%d %d\n", PrevTotal, Total);

    // differentiate: actual value minus the previous one
    int totald = Total - PrevTotal;
    int idled = Idle - PrevIdle;
    printf("%d %d\n", totald, idled);

    printf("%d\n", totald - idled);

    float CPU_Percentage = 100*(totald - idled)/totald;

    return CPU_Percentage;
}

short CheckCoreCount()
{
    FILE* file = fopen("/proc/cpuinfo", "r");
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

