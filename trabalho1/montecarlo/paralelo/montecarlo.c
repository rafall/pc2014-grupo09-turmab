#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 16
#define MAX_INTERACTION (1E+09)
#define NUM_INTERACTION (MAX_INTERACTION/NUM_THREADS)

const int A = 1103515245, C = 12345, m = (1<<30);
const long long T = (long long) m*m;
pthread_t callThd[NUM_THREADS];
long long int *hits;

int next(int& x)
{
        return x = (x*A+C)%m;
}

void *calculate_pi(void *arg) {

        unsigned long long int i = 0;
        long offset;
        int x = time(NULL);

        offset = (long)arg;

        for (; i < NUM_INTERACTION; i++) {
                long double a = next(x);
                long double b = next(x);

                if ((a * a)+(b * b) <= T) {
                        hits[offset]++;
                }
        }

        pthread_exit((void*) EXIT_SUCCESS);
}

int main(void) {

        long i;
        long long int sum_hits=0;
        hits = (long long int *)malloc(sizeof(long long int)*NUM_THREADS);

        for (i = 0; i < NUM_THREADS; i++) {
                hits[i] = 0;
                pthread_create(&callThd[i], NULL, calculate_pi, (void *) i);
        }

        for (i = 0; i < NUM_THREADS; i++) {
                pthread_join(callThd[i], NULL);
                sum_hits += hits[i];
        }

        printf("%.6lf\n", sum_hits*4/MAX_INTERACTION);

        free(hits);
        return (EXIT_SUCCESS);
}
