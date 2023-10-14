#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpu.h"


int main()
{
    CheckCoreCount();
    return 0;
    FILE* file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Can't read /proc/stat!");
        return 2;
    }

    CPUStats cpu, prev;
    int i;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), file)) {

        // if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4))
        if(!strncmp(buffer, "cpu2", 4))
        {
            sscanf(buffer, "cpu%d %d %d %d %d %d %d %d %d %d %d", 
            &i, 
            &prev.user, 
            &prev.nice,
            &prev.system,
            &prev.idle,
            &prev.iowait,
            &prev.irq,
            &prev.softirq,
            &prev.steal,
            &prev.guest,
            &prev.guest_nice);
            printf("%s", buffer);
            break;
        }
    }
    fclose(file);
    usleep(1000000);

    file = fopen("/proc/stat", "r");
    if (!file) {
        printf("Can't read /proc/stat!");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file)) {

        // if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4))
        if(!strncmp(buffer, "cpu2", 4))
        {
            sscanf(buffer, "cpu%d %d %d %d %d %d %d %d %d %d %d", 
            &i, 
            &cpu.user, 
            &cpu.nice,
            &cpu.system,
            &cpu.idle,
            &cpu.iowait,
            &cpu.irq,
            &cpu.softirq,
            &cpu.steal,
            &cpu.guest,
            &cpu.guest_nice);

            printf("%s", buffer);
            printf("%f\n", CalculateCpuUsage(cpu));
            break;
        }
    }

    fclose(file);

    return 0;
}

// TODO: 
// Read /proc/stat
// Calculate processor usage
// Print it
// Create 3 threads with pthread
// Connect reader and analyzer
// Connect analyzer with printer