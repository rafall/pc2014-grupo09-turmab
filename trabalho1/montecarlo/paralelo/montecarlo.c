#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 4
#define MAX_INTERACTION (1E+06)
#define NUM_INTERACTION (MAX_INTERACTION/NUM_THREADS)

pthread_t callThd[NUM_THREADS];
long long int *hits;

/*double gaussrand()
{
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) {
		do {
			double U1 = (double)drand48() / RAND_MAX;
			double U2 = (double)drand48() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
			} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else
		X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X;
}*/

void *calculate_pi(void *arg) {
    unsigned long int i = 0;
    long offset;
    
    offset = (long) arg;
    
    for (; i < NUM_INTERACTION; i++) {
        long double x = drand48();
        long double y = drand48();
        
/*        long double x = gaussrand();
        long double y = gaussrand();
*/      
        if ((x * x)+(y * y) < 1) {
            hits[offset]++;
        }
    }

    pthread_exit((void*) EXIT_SUCCESS);
}

int main(void) {
    long i;
    void *status=0;
    long long int sum_hits=0;
    hits = (long long int *)malloc(sizeof(long long int)*NUM_THREADS);

    srand48(time(NULL));


    for (i = 0; i < NUM_THREADS; i++) {
        hits[i] = 0;
        pthread_create(&callThd[i], NULL, calculate_pi, (void *) i);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(callThd[i], (void *) status);
        printf("hits: %lli\n", hits[i]);
        sum_hits += hits[i];
    }

    printf("PI =  %lf \n", sum_hits/NUM_INTERACTION);

    free(hits);
    return (EXIT_SUCCESS);
}

