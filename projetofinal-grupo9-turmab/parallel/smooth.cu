/* ----------------------------------------------------------------- */
/*  Copyright (c) 2014                                               */
/*             Fernando Noveletto Candiani         7239131,          */
/*             Marcius Leandro Junior              7277433,          */
/*             Rafael Hiroki de Figueiroa Minami   7573187,          */
/*                                                                   */
/*  This program is free software; you can redistribute it and/or    */
/*  modify it under the terms of the GNU General Public License as   */
/*  published by the Free Software Foundation; either version 3 of   */
/*  the License, or (at your option) any later version.  See the     */
/*  file LICENSE included with this distribution for more            */
/*  information.                                                     */
/*  email: fncandiani, marcius, rafahiroki @usp.br                   */
/* ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*Show debug messages*/
#define _DEBUG_ 0

/*Variables used to generate errors*/
#define _FAILURE_ 0
#define _SUCCESS_ 1
#define _MEM_ERR_ 2

/*Default filter size*/
#define _FILT_SIZE_ 5
#define _SIZE_F_ 1024.0
#define _SIZE_ 1024

/*Value that is used to transform from microseconds to seconds.*/
#define _SEC_TO_USEC_ 1000000

/*shorter string for the unsigned short int type.*/
#define usint unsigned short int

/*Returns the difference between t0 and t1, i.e. t1-t0 in microseconds.*/
double time_diff(struct timeval t0, struct timeval t1){
	return ((double)t1.tv_sec*_SEC_TO_USEC_ + (double)t1.tv_usec)
		  -((double)t0.tv_sec*_SEC_TO_USEC_ + (double)t0.tv_usec);
}

__global__ void col_add( int *img, int *cols_sum, int rows, int cols ) {
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	if(tid < (rows)*(cols+4)){ 
		cols_sum[tid] = img[tid]+
					img[tid+(cols+4)]+
					img[tid+(2*(cols+4))]+
					img[tid+(3*(cols+4))]+
					img[tid+(4*(cols+4))];
	}
}

__global__ void smooth(int *sum,int *out,int rows, int cols, int *c){

	int tid = blockIdx.x * blockDim.x + threadIdx.x ; // cria id único para cada thread

	if(tid < rows*cols){ // faz as somas necessárias
		out[tid] = sum[tid+2+(4*(tid/cols))-2] + 
				sum[tid+2+(4*(tid/cols))-1] + 
				sum[tid+2+(4*(tid/cols))] +
				sum[tid+2+(4*(tid/cols))+1] +
				sum[tid+2+(4*(tid/cols))+2] ;
		
		// divide a soma pelo tamanha da mascara
		out[tid] /= _FILT_SIZE_*_FILT_SIZE_;

	}
}

int main(int argc, char** argv){

	usint i = 0, j = 0, rows = 0, cols = 0, filt_size = 0, max_val = 0;
	char format[3], img_name[500], hash[2];
	int *in_r = NULL, *in_g = NULL, *in_b = NULL, *out_r = NULL, *out_g = NULL, *out_b = NULL;

	/*Structure that is used to calculate the time that is taken to */
	/*to execute the algorithm. We use this approach instead of the */
	/*bash program /usr/bin/time -f "%e" because we can't consider  */
	/*the reading and writing time, i.e. the reading (scanf) and    */
	/*write (printf) takes the same amount of time in the parallel  */
	/*and sequential programs and since the speed up function is a  */
	/*non-linear function this will change the real value of the    */
	/*speed up. For example, if the sequential algorithm, including */
	/*the reading and writing, takes 20 seconds to calculate the    */
	/*smooth of an image and the parallel takes 15 seconds; the     */
	/*speed up here would be 1.33, but let's say the reading and    */
	/*writing takes 10 seconds, so the real computation time of each*/
	/*approach would be 10 and 5 seconds, respectively. In this case*/
	/*the speed up difference would be 0.67. If we consider that    */
	/*this algorithm can run for a long period of time this value   */
	/*would get bigger and bigger, so that is why we use this       */
	/*different approach.*/
	struct timeval t0, t1;

	/*Read from the parameters if they changed the filter size, if  */
	/*not use the default value of 5.                               */
	if(2 > argc){
		if(_DEBUG_) printf("No filter size provided, using default.\n");
		filt_size = _FILT_SIZE_;
	}else{
		//filt_size = (usint) atoi(argv[1]);
		filt_size = _FILT_SIZE_;
	}

	/*Start reading the image as ppm from the stdin*/
	scanf("%s%s%s", format, hash, img_name);
	if(_DEBUG_) printf("%s\n%s %s\n", format, hash, img_name);

	scanf("%hu%hu%hu",&cols,&rows,&max_val);
	if(_DEBUG_) printf("%hu %hu\n%hu\n",cols,rows, max_val);

	in_r = (int *) calloc((cols+4)*(rows+4),sizeof(usint));
	in_g = (int *) calloc((cols+4)*(rows+4),sizeof(usint));
	in_b = (int *) calloc((cols+4)*(rows+4),sizeof(usint));

	/*Read the matrix from the stdin.*/
	for(i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%d%d%d",&in_r[((i+filt_size/2)*(cols+filt_size-1))+j+filt_size/2],
							  &in_g[((i+filt_size/2)*(cols+filt_size-1))+j+filt_size/2],
							  &in_b[((i+filt_size/2)*(cols+filt_size-1))+j+filt_size/2]);
		}
	}

	if(_DEBUG_) for(i=0;i<rows+filt_size-1;++i){ for(j=0;j<cols+filt_size-1;++j)
				printf("%d %d %d\t", in_r[(i*(cols+filt_size/2+1))+j+filt_size/2-1],
										in_g[(i*(cols+filt_size/2+1))+j+filt_size/2-1],
										in_b[(i*(cols+filt_size/2+1))+j+filt_size/2-1]);
				printf("\n");}

	out_r = (int *) calloc(cols*rows,sizeof(usint));
	out_g = (int *) calloc(cols*rows,sizeof(usint));
	out_b = (int *) calloc(cols*rows,sizeof(usint));

	/*Images that are goin to be used in the device.*/
	int *dev_in = NULL, *dev_out = NULL;
	int *sum, *c;

	float numblocks;
	int blocks;

	if(cols > rows){
		numblocks = (cols/_SIZE_F_)*rows;
	}
	else{
		numblocks = (rows/_SIZE_F_)*cols;
	}

	blocks = (int) (numblocks + 1.0);


	gettimeofday(&t0, NULL);

	/*Allocates the memory needed in the device.*/
	cudaMalloc( (void**)&dev_in, (rows+filt_size-1)*(cols+filt_size-1)*sizeof(int));
	cudaMalloc( (void**)&dev_out, (rows)*(cols) * sizeof(int));
	cudaMalloc( (void**)&sum,(rows*(cols+4))*sizeof(int));
	cudaMalloc( (void**)&c,(1*sizeof(int)));

	/*Copy the image from the host to the device (CPU -> GPU)*/
	cudaMemcpy( dev_in, in_r, (rows+filt_size-1)*(cols+filt_size-1)*sizeof(int), cudaMemcpyHostToDevice);
	col_add<<<blocks,_SIZE_>>>(dev_in,sum,rows,cols);
	smooth<<<blocks,_SIZE_>>>(sum,dev_out,rows,cols,c);
	cudaMemcpy(out_r, dev_out, rows*cols*sizeof(int), cudaMemcpyDeviceToHost);
	
	/*Copy the image from the host to the device (CPU -> GPU)*/
	cudaMemcpy( dev_in, in_g, (rows+filt_size-1)*(cols+filt_size-1)*sizeof(int), cudaMemcpyHostToDevice);
	col_add<<<blocks,_SIZE_>>>(dev_in,sum,rows,cols);
	smooth<<<blocks,_SIZE_>>>(sum,dev_out,rows,cols,c);
	cudaMemcpy(out_g, dev_out, rows*cols*sizeof(int), cudaMemcpyDeviceToHost);

	/*Copy the image from the host to the device (CPU -> GPU)*/
	cudaMemcpy( dev_in, in_b, (rows+filt_size-1)*(cols+filt_size-1)*sizeof(int), cudaMemcpyHostToDevice);
	col_add<<<blocks,_SIZE_>>>(dev_in,sum,rows,cols);
	smooth<<<blocks,_SIZE_>>>(sum,dev_out,rows,cols,c);
	cudaMemcpy(out_b, dev_out, rows*cols*sizeof(int), cudaMemcpyDeviceToHost);

//	cudaDeviceSynchronize();

	/*Copy the result from the device to the host. (GPU -> CPU)*/

	/*Point where the timer is stopped.*/
	gettimeofday(&t1, NULL);

	fprintf(stderr,"%.0lf\n",time_diff(t0,t1));

	/*print the image as ppm to the stdout*/
	printf("%s\n%s %s\n",format, hash, img_name);
	printf("%hu %hu\n%hu\n", cols, rows, max_val);
	for( i = 0; i < rows; i++){
		for(j=0;j< cols-1; j++){
			printf("%d %d %d   ", out_r[i*cols+j],out_g[i*cols+j],out_b[i*cols+j]);
		}
		printf("%d %d %d	", out_r[i*cols+j],out_g[i*cols+j],out_b[i*cols+j]);
		printf("\n");
	}


	free(in_r);
	free(in_g);
	free(in_b);

	free(out_r);
	free(out_g);
	free(out_b);

	cudaFree(dev_in);
	cudaFree(dev_out);
	cudaFree(sum);

	return EXIT_SUCCESS;
}
