/* ----------------------------------------------------------------- */
/*  Copyright (c) 2014                                               */
/*  Fernando Noveletto Candiani         7239131,                     */
/*  Marcius Leandro Junior              7277433,                     */
/*  Rafael Hiroki de Figueiroa Minami   7573187,                     */
/*                                                                   */
/*  This program is free software; you can redistribute it and/or    */
/*  modify it under the terms of the GNU General Public License as   */
/*  published by the Free Software Foundation; either version 3 of   */
/*  the License, or (at your option) any later version.  See the     */
/*  file.                                                            */
/*  LICENSE included with this distribution for more information.    */
/*  email: fncandiani, marcius, rafahiroki @usp.br                   */
/* ----------------------------------------------------------------- */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

int main (void){

	int n,m,p,i,j;
	scanf("%d%d%d",&n,&m,&p);

	int *a = (int *) malloc(sizeof(int)*n*m);
	int *b = (int *) malloc(sizeof(int)*m*p);

	for(i = 0; i < n; i++){
		for(j = 0; j < m; j++){
			scanf("%d",&a[i*m+j]);
			if(DEBUG) printf("%d ",a[i*m+j]);
		}
		if(DEBUG) printf("\n");
	}

	if(DEBUG) printf("\n");

	for(i = 0; i < m; i++){
		for(j = 0; j < p; j++){
			scanf("%d",&b[i*p+j]);
			if(DEBUG) printf("%d ",b[i*p+j]);
		}
		if(DEBUG) printf("\n");
	}
	if(DEBUG) printf("\n");


	free(a);
	free(b);

	return EXIT_SUCCESS;
}

