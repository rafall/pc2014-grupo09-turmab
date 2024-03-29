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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define max(a,b) ({ \   /* retorna o maior entre a e b */
	typeof(a) _a_temp_; \
	typeof(b) _b_temp_; \
	_a_temp_ = (a); \
	_b_temp_ = (b); \
	_a_temp_ = _a_temp_ < _b_temp_ ? _b_temp_ : _a_temp_; \
})

const int A = 1103515245, C = 12345, m = (1<<30);
pthread_t *callThd;
unsigned long int num_threads, M = 0;
long double S = 0.0, E = 0.0, r = 0.0, sigma = 0.0, T = 0.0, mean = 0.0, *trials, *sum, *squared_deviation;

int next(int& x){   /* gera um numero aleatorio */
	x = (x*A+C)%m;
	if (x < 0)
		x = (x+m)%m;
	return x;
}

void* blackscholes(void *arg){    /* Modelo de Black Scholes atraves da simulacao de Monte Carlo */

	long long int offset, size = M/num_threads;
	int x = time(NULL);

	offset = (unsigned long int) arg;

	unsigned long long int i = 0;
	unsigned long long int begin = offset*size;
	unsigned long long int end = (offset+1)*size;

	for(i = begin; i < end; i++){
		double t = S * exp((r - 0.5*sigma*sigma) * T + sigma * sqrt(T) * next(x) / m);
        trials[i] = exp(-r * T) * max(t - E, 0);
		sum[offset] += trials[i];
	}

	pthread_exit((void*) EXIT_SUCCESS);
}

void *standard_deviation(void* arg){    /* calculo do desvio padrao */

	long long int offset, size = M/num_threads;

	if(0 == size)
		pthread_exit((void*) EXIT_SUCCESS);

	offset = (long long int )arg;

	long double mean_difference = 0.0;
	unsigned long long int i = 0;
	unsigned long long int begin = offset*size;
	unsigned long long int end = (offset+1)*size;

	for(i = begin; i < end; i++){
		mean_difference = trials[i] - mean;
		squared_deviation[offset] += mean_difference*mean_difference;
	}

	pthread_exit((void*) EXIT_SUCCESS);
}

int main(int argc, char** argv){


	if(argc < 2)
		num_threads = 4;
	else
		num_threads = atoi(argv[1]);

	scanf("%Lf", &S);       /*valor da acao  */
	scanf("%Lf", &E);       /* preco de exercicio da opcao */
	scanf("%Lf", &r);       /* taxa de juros livre de risco */
	scanf("%Lf", &sigma);   /* volatilidade da acao */
	scanf("%Lf", &T);       /* tempo de validade da opcao */
	scanf("%ld", &M);       /* numero de iteracoes */

	unsigned long int i;
	long double sum_hits = 0.0;
	long double std_deviation = 0.0;

	trials = (long double*) malloc(sizeof(long double)*M);
	sum = (long double*) malloc(sizeof(long double)*num_threads);
	squared_deviation = (long double*)malloc(sizeof(long double)*num_threads);
	callThd = (pthread_t *)malloc(sizeof(pthread_t)*num_threads);

/*	printf("%Lf, %Lf, %Lf, %Lf, %Lf, %ld\n", S, E, r, sigma, T, M);*/

	for (i = 0; i < num_threads; i++) {     /* cria as threads para a funcao blackscholes */
		pthread_create(&callThd[i], NULL, blackscholes, (void *) i);
	}

	for (i = 0; i < num_threads; i++) {
                pthread_join(callThd[i], NULL);     /* as threads devem terminar juntas */
		sum_hits += sum[i];
	}

	mean = sum_hits / M;

	for (i = 0; i < num_threads; i++) {     /* cria as threads para a funcao standard_deviation */
		pthread_create(&callThd[i], NULL, standard_deviation, (void *) i);
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(callThd[i], NULL);     /* as threads devem terminar juntas */
		std_deviation += squared_deviation[i];
	}

	long double confidence_interval = 1.96*(sqrt(std_deviation/M)/sqrt(M));

/*	printf("The confidence interval calculated is [%Lf,%Lf]\n", mean - confidence_interval, mean + confidence_interval);*/

	printf("%.6Lf\n%.6Lf\n", mean, confidence_interval);

	free(trials);
	free(sum);
	free(squared_deviation);
	free(callThd);

	return EXIT_SUCCESS;
}
