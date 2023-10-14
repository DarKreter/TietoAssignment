#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

void* Reader(void* _pipe)
{
    int *pipefd = (int*)(_pipe);
    CPUStats cpu;
    char buffer[128];

    while(1)
    {
        FILE* file = fopen("/proc/stat", "r");
        if (!file) {
            perror("Can't read /proc/stat!");
            exit(2);
        }
        while (fgets(buffer, sizeof(buffer), file)) {
            if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4))
            {
                sscanf(buffer, "cpu%hd %d %d %d %d %d %d %d %d %d %d", &cpu.id, &cpu.user, &cpu.nice, &cpu.system, &cpu.idle, &cpu.iowait, &cpu.irq, &cpu.softirq, &cpu.steal, &cpu.guest, &cpu.guest_nice);
                write(pipefd[1], &cpu, sizeof(cpu));
            }
        }
        fclose(file);

        sleep(1);
    }

    return NULL;
}

void *Analyzer(void* _pipe)
{
    int *pipefd = (int (*))_pipe;
    int cc = CheckCoreCount();
    int n;
    CPUStats cpu;
  
    while (1) {
        for (int i = 0; i < cc;i++)
        {
            n = read(pipefd[0], &cpu, sizeof(cpu));
            if (n <= 0) break;
            
            printf("Got: cpu%hd %d %d %d %d %d %d %d %d %d %d\n", cpu.id,cpu.user, cpu.nice,cpu.system,cpu.idle,cpu.iowait,cpu.irq,cpu.softirq,cpu.steal,cpu.guest,cpu.guest_nice);

        }
        printf("\n");
    }

  return NULL;
}