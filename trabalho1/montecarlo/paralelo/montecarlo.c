#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 4
#define MAX_INTERACTION (1E+09)

pthread_t callThd[NUM_THREADS];
long long int *hits;
unsigned long int num_interctions_threads;

void *calculate_pi(void *arg) {
    unsigned long int i = 0;
    long offset;
    
    offset = (long) arg;
    
    for (; i < num_interctions_threads; i++) {
        long double x = drand48();
        long double y = drand48();
        
        if ((x * x)+(y * y) < 1) {
            hits[offset]++;
        }
    }

    pthread_exit((void*) EXIT_SUCCESS);
}

void montecarlo(unsigned long int num_interactions){
    long i;
    void *status=0;
    long long int sum_hits=0;
    hits = (long long int *)malloc(sizeof(long long int)*NUM_THREADS);

    srand48(time(NULL));
    num_interctions_threads /= num_interactions;

    for (i = 0; i < NUM_THREADS; i++) {
        hits[i] = 0;
        pthread_create(&callThd[i], NULL, calculate_pi, (void *) i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(callThd[i], (void *) status);
        sum_hits += hits[i];
    }

    //printf("PI =  %lf \n", sum_hits/NUM_INTERACTION);

    free(hits);
}

int main(void) {
    unsigned long int i = 1E+7;
	time_t ini;

    for(; i <= MAX_INTERACTION; i*=10){
        srand48(time(NULL));
   		ini = time(0);
		montecarlo(i);
		printf("%d\n",(int)(time(0)-ini));
	}
	
    return (EXIT_SUCCESS);
}

