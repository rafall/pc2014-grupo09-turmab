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
#include <time.h>

#define MAX_INTERACTION 1E+09

void montecarlo(unsigned long int num_interaction){
    unsigned long int i = 0, hits = 0;;
	long double x = 0, y = 0;

	for(; i < num_interaction; i++){
		x = drand48();       /*gera um valor aleatorio para x*/
		y = drand48();       /*gera um valor aleatorio para y*/

		if((x*x)+(y*y) < 1) /*verifica se o ponto aleatorio gerado esta dentro da circunferencia*/
			hits++;

	}

	printf("%.6lf\n", 4 * (hits/MAX_INTERACTION) );
}

int main(void){
    srand48(time(NULL));  //semente para gerar os numeros aleatorios

	montecarlo(MAX_INTERACTION); /*funcao que encontra o valor de pi utilizando o metodo de monte carlo*/

	return EXIT_SUCCESS;
}
