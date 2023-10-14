#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "cpu.h"

int pipefd[2]; 
pthread_mutex_t mutex;
pthread_cond_t condvar;

void* Reader()
{
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
                sscanf(buffer, "cpu%hd %d %d %d %d %d %d %d %d %d %d", 
                &cpu.id, 
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
                write(pipefd[1], &cpu, sizeof(cpu));
            }
        }
        fclose(file);
        
        // Send EOF packet
        // char eof[2] = {0xFF, 0xFF}; 
        // write(pipefd[1], eof, 2);
        sleep(1);
    }

    return NULL;
}

void *Analyzer()
{
    int cc = CheckCoreCount();
    int n;
    CPUStats cpu;
  
    while (1) {
        for (int i = 0; i < cc;i++)
        {
            n = read(pipefd[0], &cpu, sizeof(cpu));
            if (n <= 0) break;
            
            printf("Got: cpu%hd %d %d %d %d %d %d %d %d %d %d\n", 
                cpu.id,
                cpu.user, 
                cpu.nice,
                cpu.system,
                cpu.idle,
                cpu.iowait,
                cpu.irq,
                cpu.softirq,
                cpu.steal,
                cpu.guest,
                cpu.guest_nice);

        }
        printf("\n");
    }

  return NULL;
}

int main()
{
    // short coreCount = CheckCoreCount();
    pthread_t reader, analyzer;
    pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condvar, NULL);

    // Create pipe
    pipe(pipefd);  

    pthread_create(&reader, &attr, Reader, NULL);
    pthread_create(&analyzer, &attr, Analyzer, NULL);


    /* Clean up */
	pthread_attr_destroy(&attr);

	// See note in list.hpp for full meaning of this creation
    pthread_join(reader, NULL);
    pthread_join(analyzer, NULL);
    // pthread_exit(NULL);


    // usleep(1000000);

    // file = fopen("/proc/stat", "r");
    // if (!file) {
    //     printf("Can't read /proc/stat!");
    //     return 1;
    // }

    // while (fgets(buffer, sizeof(buffer), file)) {

    //     // if(!strncmp(buffer, "cpu", 3) && strncmp(buffer, "cpu ", 4))
    //     if(!strncmp(buffer, "cpu2", 4))
    //     {
    //         sscanf(buffer, "cpu%d %d %d %d %d %d %d %d %d %d %d", 
    //         &i, 
    //         &cpu.user, 
    //         &cpu.nice,
    //         &cpu.system,
    //         &cpu.idle,
    //         &cpu.iowait,
    //         &cpu.irq,
    //         &cpu.softirq,
    //         &cpu.steal,
    //         &cpu.guest,
    //         &cpu.guest_nice);

    //         printf("%s", buffer);
    //         printf("%f\n", CalculateCpuUsage(cpu));
    //         break;
    //     }
    // }

    // fclose(file);

    return 0;
}

// TODO: 
// Read /proc/stat
// Calculate processor usage
// Print it
// Create 3 threads with pthread
// Connect reader and analyzer
// Connect analyzer with printer