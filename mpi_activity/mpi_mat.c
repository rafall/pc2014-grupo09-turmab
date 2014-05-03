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

#define DEBUG 1
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

int main(int argc, char *argv[]){

	/*All the variables needed to control the multiplication of the 2 matrices*/
	int n,m,p,i,j,task, rowsA, offset, ntask, nworker, rc = 0;
	/*AxB=C - all the matrices needed*/
	double *b, *a, *c;
	MPI_Status status;

	/*Init the MPI parameters*/
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&task);
	MPI_Comm_size(MPI_COMM_WORLD,&ntask);

	/*Check to see wether the program is executing in more than one machine*/
	if(ntask < 2 ){
		printf("To run an MPI program 2 or more tasks are needed.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(EXIT_FAILURE);
	}

	nworker = ntask-1;

	/*Allocates the memory need to read and compute*/
	a = (double *) malloc(sizeof(double)*n*m);
	b = (double *) malloc(sizeof(double)*m*p);
	c = (double *) malloc(sizeof(double)*n*p);

	/*Start the master activity*/
	if(MASTER == task){
		scanf("%d%d%d",&n,&m,&p);

		for(i = 0; i < n; i++){
			for(j = 0; j < m; j++){
				scanf("%lf",&a[i*m+j]);
				if(DEBUG) printf("%lf ",a[i*m+j]);
			}
			if(DEBUG) printf("\n");
		}
		if(DEBUG) printf("\n");

		for(i = 0; i < m; i++){
			for(j = 0; j < p; j++){
				scanf("%lf",&b[i*p+j]);
				if(DEBUG) printf("%lf ",b[i*p+j]);
			}
			if(DEBUG) printf("\n");
		}
		if(DEBUG) printf("\n");

		offset = 0;
		int destination = 1;

		/*Send the data needed in the slave nodes to compute the multiplication*/
		for(; destination <= nworker; destination++){
			rowsA = (destination <= n%nworker) ? (n/nworker)+1 : n/nworker;
			MPI_Send(&offset, 1, MPI_INT, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&rowsA, 1, MPI_INT, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&n, 1, MPI_INT, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&p, 1, MPI_INT, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&a[offset*m], rowsA*m,  MPI_DOUBLE, destination, FROM_MASTER, MPI_COMM_WORLD);
			MPI_Send(&b, m*p,  MPI_DOUBLE, destination, FROM_MASTER, MPI_COMM_WORLD);
			offset = offset + rowsA;
		}

		/*Receive the data of the computed multiplication*/
		for(i=1; i<=nworker; i++){
			MPI_Recv(&offset, 1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
			MPI_Recv(&rowsA, 1, MPI_INT, i, FROM_WORKER, MPI_COMM_WORLD, &status);
			MPI_Recv(&c[offset*m], rowsA*p, MPI_DOUBLE, i, FROM_WORKER, MPI_COMM_WORLD, &status);
		}

		/*Print out the result calculated*/
		for(i = 0; i < n; i++){
			for(j = 0; j < p; j++){
				printf("%lf ",c[i*p+j]);
			}
			printf("\n");
		}
		printf("\n");
	}

	/*Start all nodes that are going to process the multiplication*/
	if(task > MASTER){

		int k;
		/*Receive all the data needed to compute the multiplication of the two matrices*/
		MPI_Recv(&offset, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&rowsA, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&n, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&m, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&p, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&a, rowsA*m, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, m*p, MPI_DOUBLE, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);

		/*Calculate the multiplication of the rows of A by B adding it to C*/
		for(k = 0; k < p; k++){
			for(i = 0; i < rowsA; i++){
				c[i*p+k] = 0.0;
				for(j=0; j<m; j++){
					c[i*p+k] = c[i*p+k] + a[i*n+j] * b[j*p+k];
				}
			}
		}

		/*Send the result calculated back to the master.*/
		MPI_Send(&offset, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
		MPI_Send(&rowsA, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
		MPI_Send(&c, rowsA*p, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);
	}

	/*Free the used memory*/
	free(a);
	free(b);
	free(c);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
