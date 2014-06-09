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

/*Show debug messages*/
#define _DEBUG_ 0

/*Variables used to generate errors*/
#define _FAILURE_ 0
#define _SUCCESS_ 1
#define _MEM_ERR_ 2

/*Default filter size*/
#define _FILT_SIZE_ 3

/*shorter string for the unsigned short int type.*/
#define usint unsigned short int

/*Structure that will save all the image data, i.e. the RBG colors.*/
typedef struct image image;

struct image{
	usint **r, **g, **b;
};

/*The matrices that are constructed here are colums by rows, not the   */
/*usual rows by columns.*/

void allocate_img(image *img, usint cols, usint rows){

	usint i;

	/*Allocates the memory needed for the image*/
	img->r = (usint **) calloc(cols,sizeof(usint *));
	img->g = (usint **) calloc(cols,sizeof(usint *));
	img->b = (usint **) calloc(cols,sizeof(usint *));

	for(i = 0; i < cols; i++){
		img->r[i] = (usint *) calloc(rows,sizeof(usint *));
		img->g[i] = (usint *) calloc(rows,sizeof(usint *));
		img->b[i] = (usint *) calloc(rows,sizeof(usint *));
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

	/*Read from the parameters if they changed the filter size, if  */
	/*not use the default value of 5.                               */
	if(2 > argc){
		if(_DEBUG_) printf("No filter size provided, using default.\n");
		filt_size = _FILT_SIZE_;
	}else{
		filt_size = (usint) atoi(argv[1]);
	}

	/*Start reading the image as ppm from the stdin*/
	scanf("%s%s%s", format, hash, img_name);
	if(_DEBUG_) printf("%s\n%s %s\n", format, hash, img_name);

	scanf("%hu%hu%hu",&cols,&rows,&max_val);
	if(_DEBUG_) printf("%hu %hu\n%hu\n",cols,rows, max_val);

	in = (image *) malloc(sizeof(image));
	allocate_img(in, cols+filt_size-1, rows+filt_size-1);

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

	out = (image *) malloc(sizeof(image));
	allocate_img(out, cols, rows);

	//1 - copiar as matrizes para a GPU (nao esquecer de alocar memoria na GPU)
	//2 - mais as flags necessarias (numero de threads, blocks, vao ser 3 grids (RGB))
	//3 - copiar para a shared memory a parte da matriz q vai (syncthreads)
	//usar pra fazer o calculo (isso eh na funcao que vai ser executada na gpu)
	//4 - inicializar 3 kernels (funcoes q vao ser executada na GPU) um para cada
	// espectro de cor (RGB) - isso eh assincrono
	//5 - Pesquisar com vai esperar o resultado dos kernels para poder salvar a imagem
	// em disco

	//essa eh a funcao q vai executar na GPU
	//localfilt(in, out, rows, cols, filt_size);

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
