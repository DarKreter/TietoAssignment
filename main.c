#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct CPUStats{
    int user;
    int nice;
    int system;
    int idle;
    int iowait;
    int irq;
    int softirq;
    int steal;
    int guest;
    int guest_nice;
};

float CalculateCpuUsage(struct CPUStats prev, struct CPUStats curr)
{
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


int main(int argc, char** argv)
{
    FILE* file = fopen("/proc/stat", "r");
    if (!file) {
        printf("Can't read /proc/stat!");
        return 1;
    }

    struct CPUStats cpu, prev;
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
            printf("%f\n", CalculateCpuUsage(prev, cpu));
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