/* ----------------------------------------------------------------- */
/*  Copyright (c) 2014 Fernando Noveletto Candiani, Marcius Leandro  */
/*  Junior, Rafael Hiroki de Figueiroa Minami                        */
/*                                                                   */
/*  This program is free software; you can redistribute it and/or    */
/*  modify it under the terms of the GNU General Public License as   */
/*  published by the Free Software Foundation; either version 3 of   */
/*  the License, or (at your option) any later version.  See the     */
/*  file.                                                            */
/*  LICENSE included with this distribution for more information.    */
/*  email: fncandiani, marcius, rafahiroki @usp.br                   */
/* ----------------------------------------------------------------- */

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

int next(int& x) /*funcao que gera numeros aleatorios*/
{
	return x = (x*A+C)%m;
}

void *calculate_pi(void *arg) {

	unsigned long long int i = 0;
	long offset;
	int x = time(NULL);

	offset = (long)arg;

	for (; i < num_interaction; i++) { /*Gera um ponto (x, y) aleatorio */
		long double a = next(x);
		long double b = next(x);

		if ((a * a)+(b * b) <= T) {  /*verifica se esse ponto está dentro da circunferencia*/
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
		pthread_create(&callThd[i], NULL, calculate_pi, (void *) i); /*cria as threads que executarao a funcao calculate_pi*/
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(callThd[i], NULL);   /*as threads terminam juntas*/
		sum_hits += hits[i];
	}

	printf("%.6lf\n", sum_hits * 4/MAX_INTERACTION);  /* pi = 4*soma dos pontos dentro da circunferencia/quantidade de pontos */

	free(hits);
	free(callThd);
	return (EXIT_SUCCESS);
}
