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
#include <time.h>
#include <math.h>

#define max(a,b) ({ \
	typeof(a) _a_temp_; \
	typeof(b) _b_temp_; \
	_a_temp_ = (a); \
	_b_temp_ = (b); \
	_a_temp_ = _a_temp_ < _b_temp_ ? _b_temp_ : _a_temp_; \
})

int main(void){

	long double S = 0.0, E = 0.0, r = 0.0, sigma = 0.0, T = 0.0, *trials;
	long long double sum = 0.0;
	long int M = 0, i = 0;
	unsigned short index[3];

	scanf("%Lf", &S);
	scanf("%Lf", &E);
	scanf("%Lf", &r);
	scanf("%Lf", &sigma);
	scanf("%Lf", &T);
	scanf("%ld", &M);

	trials = (long double*) malloc(sizeof(long double)*M);

	printf("%Lf, %Lf, %Lf, %Lf, %Lf, %ld\n", S, E, r, sigma, T, M);

	index[0] = time(NULL);
	index[1] = time(NULL);
	seed48(index);

	for( ;i < M; i++){
		long double rand = erand48(index);
		long double t = S * exp( ( (r - ((sigma*sigma)/2)) * T ) + (sigma*sqrt(T)*rand) );
		trials[i] = exp( (-1*r)*T) * max(t-E, 0);
		sum += trials[i];
	}

	free(trials);

	return EXIT_SUCCESS;
}