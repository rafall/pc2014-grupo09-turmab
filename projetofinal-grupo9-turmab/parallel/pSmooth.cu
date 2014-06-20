#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum channels {RED, GREEN, BLUE};

#define usint unsigned short int;

#define THREADS_PER_BLOCK 1024

__global__ void smooth(usin *image, usint *smoothedImage, usint imageHeight, usint imageWidth)
{
	int row = blockIdx.y*blockDim.y + threadIdx.y;
	int col = blockIdx.x*blockDim.x + threadIdx.x;
	int index = col + row*THREADS_PER_BLOCK;

	int k, l;

	/*If it's out of the image it doesn't need to do anything*/
	if(row > imageHeight || col > imageWidth)
		return;

	/*Smooth on a single pixel by getting the mean of a 5x5 square of pixels*/
	for(k = row - 2; k <= row + 2; k++)
	{
		for(l = col - 2; l <= col + 2; l++)
		{
			/*If it's trying to reach a pixel that's out of the image
			 *we consider it's value as the max color
			 */
			if(k < 0 || l < 0 || k >= imageHeight || l >= imageWidth)
			{
				mean += maxColor;
			}
			else
			{
				usint tInx = l + k*THREADS_PER_BLOCK;
				mean += image[tIdx];
			}	
		}
	}

	smoothedImage[index] = mean;

}

int main ()
{
	/*HOST variables*/
	usint *in[3], *out[3];
	char str[30];
	usint maxColor;
	usint imageHeight, imageWidth;
	usint numOfBlocks;
	int i;

	/*DEVICE variables*/
	usint dev_imageHeight, dev_imageWidth;
	usint *dev_in, *dev_out;

	/*Reading image header*/
	scanf("%s", str); /*PPM format*/
	scanf("%s", str); /*hash*/
	scanf("%s", str); /*name*/
	scanf("%u", &imageWidth);
	scanf("%u", &imageHeight);
	scanf("%u", &maxColor);

	/*Size of array*/
	usint size = imageHeight*imageWidth*sizeof(usint);

	/*Using the ceil function to round the number of blocks upward*/
	numOfBlocks = (usint) ceil(imageHeight*imageWidth/THREADS_PER_BLOCK);

	/*Allocating HOST memory*/
	for(i = 0; i < 3; i++)
	{
		in[i] = (int*) malloc (size);
		out[i] = (int*) malloc (size);
	}

	/*Allocating DEVICE memory*/
	cudaMalloc( (void **) &dev_in, size);
	cudaMalloc( (void **) &dev_out, size);

	/*Reading image file*/
	for(i = 0; i < imageHeight*imageWidth; i++)
	{
		scanf("%d", &p1);
		in[RED][i] = p1;
		scanf("%d", &p1);
		in[GREEN][i] = p1;
		scanf("%d", &p1);
		in[BLUE][i] = p1;
	}
	
	/*Copy input to device*/
	cudaMemcpy(dev_imageWidth, &imageWidth, size, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_imageHeight, &imageHeight, size, cudaMemcpyHostToDevice);

	/*One for each color channel*/
	for(i = 0; i < 3; i++)
	{
		/*Copy input to device*/
		cudaMemcpy(dev_in, &in[i], size, cudaMemcpyHostToDevice);
		cudaMemcpy(dev_out, &out[i], size, cudaMemcpyHostToDevice);

		/*The smooth kernel will run with numOfPixels/threadsPerBlock blocks and 1024 threads per block*/
	    smooth<<<numOfBlocks,THREADS_PER_BLOCK>>>(dev_in, dev_out, dev_imageHeight, dev_imageWidth);

	    /*Copy the result from DEVICE to HOST*/
	    cudaMemcpy(out[i], dev_out, size, cudaMemcpyDeviceToHost);
	}
	
    /*Falta criar e escrever o arquivo da imagem
	 *
	 * ....
	 *
	 */

    free(in);
    free(out);
    cudaFree(dev_in);
    cudaFree(dev_out);
    cudaFree(dev_imageWidth);
    cudaFree(dev_imageHeight);
    
    return 0;
}