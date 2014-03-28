#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_INTERACTION (1E+09)

const int A = 1103515245, C = 12345, m = (1<<30);
const long long T = (long long) m*m;
pthread_t *callThd;
unsigned long long int *hits, num_interaction;

int next(int& x)
{
	return x = (x*A+C)%m;
}

void *calculate_pi(void *arg) {

	unsigned long long int i = 0;
	long offset;
	int x = time(NULL);

	offset = (long)arg;

	for (; i < num_interaction; i++) {
		long double a = next(x);
		long double b = next(x);

		if ((a * a)+(b * b) <= T) {
			hits[offset]++;
		}
	}

	pthread_exit((void*) EXIT_SUCCESS);
}

int main(int argc, char** argv) {

	unsigned int num_threads;

	if(argc < 2)
		num_threads = 4;
	else
		num_threads = atoi(argv[1]);

	num_interaction = MAX_INTERACTION/num_threads;

	long i;
	long long int sum_hits=0;
	hits = (unsigned long long int *)malloc(sizeof(unsigned long long int)*num_threads);
	callThd = (pthread_t *)malloc(sizeof(pthread_t)*num_threads);

	for (i = 0; i < num_threads; i++) {
		hits[i] = 0;
		pthread_create(&callThd[i], NULL, calculate_pi, (void *) i);
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(callThd[i], NULL);
		sum_hits += hits[i];
	}

	printf("%.6lf\n", sum_hits*4/MAX_INTERACTION);

	free(hits);
	free(callThd);
	return (EXIT_SUCCESS);
}
