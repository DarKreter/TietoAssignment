#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "Threads.h"

// pthread_mutex_t mutex;
// pthread_cond_t condvar;

int main()
{
    pthread_t reader, analyzer;
    pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	// pthread_mutex_init(&mutex, NULL);
	// pthread_cond_init(&condvar, NULL);

    // Create pipe
    int pipeReaderAnalyzer[2];
    pipe(pipeReaderAnalyzer);  

    pthread_create(&reader, &attr, Reader, (void*)pipeReaderAnalyzer);
    pthread_create(&analyzer, &attr, Analyzer, (void*)pipeReaderAnalyzer);


    /* Clean up */
	pthread_attr_destroy(&attr);

	// See note in list.hpp for full meaning of this creation
    pthread_join(reader, NULL);
    pthread_join(analyzer, NULL);
    // pthread_exit(NULL);

    return 0;
}

// TODO: 
// Read /proc/stat
// Calculate processor usage
// Print it
// Create 3 threads with pthread
// Connect reader and analyzer
// Connect analyzer with printer