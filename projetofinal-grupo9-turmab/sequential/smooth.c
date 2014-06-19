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
#define _FILT_SIZE_ 3

/*Value that is used to transform from microseconds to seconds.*/
#define _SEC_TO_USEC_ 1000000

/*shorter string for the unsigned short int type.*/
#define usint unsigned short int

/*Structure that will save all the image data, i.e. the RBG colors.*/
typedef struct image image;

struct image{
	usint **r, **g, **b;
};

/*Returns the difference between t0 and t1, i.e. t1-t0 in microseconds.*/
double time_diff(struct timeval t0, struct timeval t1){
	return ((double)t1.tv_sec*_SEC_TO_USEC_ + (double)t1.tv_usec)
		  -((double)t0.tv_sec*_SEC_TO_USEC_ + (double)t0.tv_usec);
}


/*The matrices that are constructed here are colums by rows, not the   */
/*usual rows by columns.*/

void allocate_img(image *img, usint cols, usint rows){

	usint i;

	/*Allocates the memory needed for the image*/
	img->r = (usint **) calloc(cols,sizeof(usint *));
	img->g = (usint **) calloc(cols,sizeof(usint *));
	img->b = (usint **) calloc(cols,sizeof(usint *));

	for(i = 0; i < cols; i++){
		img->r[i] = (usint *) calloc(rows,sizeof(usint));
		img->g[i] = (usint *) calloc(rows,sizeof(usint));
		img->b[i] = (usint *) calloc(rows,sizeof(usint));
	}

}

void free_img(image *img, usint size){

	usint i;

	/*Free used memory*/
	for(i = 0; i < size; ++i){
		free(img->r[i]);
		free(img->g[i]);
		free(img->b[i]);
	}

	free(img->r);
	free(img->g);
	free(img->b);
	free(img);

}

void localfilt(image *in, image *out, usint rows, usint cols, usint filt_size){

	usint bdr_diff = filt_size/2, x, y;
	short int i,j;
	unsigned long int sum_r, sum_g, sum_b;

	for (x = bdr_diff; x < rows+bdr_diff; x++){
		for (y = bdr_diff; y < cols+bdr_diff; y++){
			sum_r = 0; sum_g = 0; sum_b = 0;
			for(i = -bdr_diff; i <= bdr_diff; i++){
				for(j = -bdr_diff; j <= bdr_diff; j++){
					sum_r += in->r[y-j][x-i];
					sum_g += in->g[y-j][x-i];
					sum_b += in->b[y-j][x-i];
				}
			}
			out->r[y-bdr_diff][x-bdr_diff] = sum_r/(filt_size*filt_size);
			out->g[y-bdr_diff][x-bdr_diff] = sum_g/(filt_size*filt_size);
			out->b[y-bdr_diff][x-bdr_diff] = sum_b/(filt_size*filt_size);
		}
	}

	if(_DEBUG_) for(i=0;i<rows;++i){ for(j=0;j<cols;++j)
				printf("%hu %hu %hu\t", out->r[j][i],out->g[j][i],out->b[j][i]); printf("\n");}

}

int main(int argc, char** argv){

	usint i = 0, j = 0, rows = 0, cols = 0, filt_size = 0, max_val = 0;
	char format[3], img_name[500], hash[2];
	image *in = NULL, *out = NULL;

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
		filt_size = (usint) atoi(argv[1]);
	}

	/*Start reading the data from the stdin*/
	scanf("%s", format);
	if(_DEBUG_) printf("%s\n", format);

	scanf("%s%s", hash, img_name);
	if(_DEBUG_) printf("%s\n", img_name);

	scanf("%hu%hu",&cols,&rows);
	if(_DEBUG_) printf("%hu %hu\n",cols,rows);

	scanf("%hu", &max_val);
	if(_DEBUG_) printf("%hu\n", max_val);

	in = (image *) malloc(sizeof(image));
	allocate_img(in, cols+filt_size-1, rows+filt_size-1);

	/*makes the border for all the colors (rgb).*/

	/*left and right border.*/
/*	for(i = 0; i < rows+filt_size-1; i++){*/
/*		for(j = 0; j < filt_size/2; j++){*/
			/*left border.*/
/*			in->r[j][i] = 0;*/
/*			in->g[j][i] = 0;*/
/*			in->b[j][i] = 0;*/
			/*right border.*/
/*			in->r[cols+filt_size-j-2][i] = 0;*/
/*			in->g[cols+filt_size-j-2][i] = 0;*/
/*			in->b[cols+filt_size-j-2][i] = 0;*/
/*		}*/
/*	}*/

	/*Top and bottom border.*/
/*	for(i = filt_size/2; i < cols+(filt_size/2);i++){*/
/*		for(j = 0; j < filt_size/2; j++){*/
			/*top border.*/
/*			in->r[i][j] = 0;*/
/*			in->g[i][j] = 0;*/
/*			in->b[i][j] = 0;*/
			/*bottom border.*/
/*			in->r[i][rows+filt_size-j-2] = 0;*/
/*			in->g[i][rows+filt_size-j-2] = 0;*/
/*			in->b[i][rows+filt_size-j-2] = 0;*/
/*		}*/
/*	}*/

	/*Read the matrix from the stdin.*/
	for(i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			scanf("%hu%hu%hu",&in->r[j+filt_size/2][i+filt_size/2],
							  &in->g[j+filt_size/2][i+filt_size/2],
							  &in->b[j+filt_size/2][i+filt_size/2]);
		}
	}

	if(_DEBUG_) for(i=0;i<rows+filt_size-1;++i){ for(j=0;j<cols+filt_size-1;++j)
				printf("%hu %hu %hu\t", in->r[j][i],in->g[j][i],in->b[j][i]); printf("\n");}

	/*Point where the timer is started.*/
	gettimeofday(&t0, NULL);

	out = (image *) malloc(sizeof(image));
	allocate_img(out, cols, rows);
	localfilt(in, out, rows, cols, filt_size);

	/*Point where the timer is stopped.*/
	gettimeofday(&t1, NULL);

	fprintf(stderr,"%.0lf\n",time_diff(t0,t1));

	/*print the image as ppm to the stdout*/
	printf("%s\n%s %s\n",format, hash, img_name);
	printf("%hu %hu\n%hu\n", cols, rows, max_val);
	for( i = 0; i < rows; i++){
		for(j=0;j< cols-1; j++){
			printf("%hu %hu %hu   ", out->r[j][i],out->g[j][i],out->b[j][i]);
		}
		printf("%hu %hu %hu", out->r[j][i],out->g[j][i],out->b[j][i]);
		printf("\n");
	}

	free_img(in, cols+filt_size-1);
	free_img(out, cols);

	return EXIT_SUCCESS;
}
