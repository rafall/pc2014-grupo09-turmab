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

#define MAX_INTERACTION 1E+09

int main(void){

	unsigned long int i = 0, hits = 0;;
	unsigned short xsubi[3];
	long double x = 0, y = 0;

	xsubi[0] = time(NULL);
	xsubi[1] = time(NULL);
	seed48(xsubi);

	for(; i < MAX_INTERACTION; i++){
		x = erand48(xsubi);
		y = erand48(xsubi);

		if((x*x)+(y*y) < 1)
			hits++;

	}

	printf("The generated pi number is %.6lf\n", 4 * (hits/MAX_INTERACTION) );
	return EXIT_SUCCESS;
}
