/* ----------------------------------------------------------------- */
/*  Copyright (c) 2013 Fernando Noveletto Candiani, Marcius Leandro  */
/*  Junior, Rafael Hiroke                                            */
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

#define max(a,b) ({ \
	typeof(a) _a_temp_; \
	typeof(b) _b_temp_; \
	_a_temp_ = (a); \
	_b_temp_ = (b); \
	_a_temp_ = _a_temp_ < _b_temp_ ? _b_temp_ : _a_temp_; \
})

double gaussrand()
{
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
			} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else
		X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X;
}

long double standard_deviation(long double *data, long int size, long double mean){

	if(0 == size)
		return 0.0;

	long double mean_difference = 0.0, squared_deviation = 0.0;
	long int i = 0;

	for(; i < size; i++){
		mean_difference = data[i] - mean;
		squared_deviation += mean_difference*mean_difference;
	}

	return sqrt(squared_deviation/size);
}

int main(void){

	long double S = 0.0, E = 0.0, r = 0.0, sigma = 0.0, T = 0.0, *trials;
	long double sum = 0.0;
	long int M = 0, i = 0;

	scanf("%Lf", &S);
	scanf("%Lf", &E);
	scanf("%Lf", &r);
	scanf("%Lf", &sigma);
	scanf("%Lf", &T);
	scanf("%ld", &M);

	trials = (long double*) malloc(sizeof(long double)*M);

	printf("%Lf, %Lf, %Lf, %Lf, %Lf, %ld\n", S, E, r, sigma, T, M);

	for( ;i < M; i++){
		long double rand = gaussrand();
		long double t = S * exp( ( (r - ((sigma*sigma)/2)) * T ) + (sigma*sqrt(T)*rand) );
		trials[i] = exp( (-1*r)*T) * max(t-E, 0);
		sum += trials[i];
	}

	printf("Sum: %Lf\n", sum);
	long double mean = sum / M;
	long double confidence_interval = 1.96*(standard_deviation(trials, M, mean)/sqrt(M));

	printf("mean %Lf, ci %Lf\n", mean, confidence_interval);
	printf("The confidence interval calculated is [%Lf,%Lf]\n", mean - confidence_interval, mean + confidence_interval);

	free(trials);

	return EXIT_SUCCESS;
}
