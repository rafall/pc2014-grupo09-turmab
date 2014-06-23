#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*Show debug messages*/
#define _DEBUG_ 1

//enum channels {RED = 0, GREEN, BLUE};

//#define usint unsigned int;

#define THREADS_PER_BLOCK 1

__global__ void smooth(unsigned int *image, unsigned int *smoothedImage, unsigned int imageHeight, unsigned int imageWidth, unsigned int maxColor)
{
	int row = blockIdx.x*blockDim.x + threadIdx.x;
	int col = blockIdx.y*blockDim.y + threadIdx.y;
	int index = imageWidth*(threadIdx.y + blockDim.y*blockIdx.y) + threadIdx.x;
	int mean = 0;
	unsigned int tIdx = 0; /*index of pixels that form the 5x5 square*/
	int k, l;

	/*If it's out of the image it doesn't need to do anything*/
	if(row >= imageHeight || col >= imageWidth)
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
				tIdx = k*imageWidth + l;
				mean += image[tIdx];
			}	
		}
	}
	smoothedImage[index] = mean/25;
	if(_DEBUG_) 
		printf("Block x: %d Block y: %d Thread: %d\nrow %d col %d image[%d] : %d\n\n",blockIdx.x, blockIdx.y, threadIdx.x, row, col, index, image[tIdx]);
}

int main ()
{
	/*HOST variables*/
	//unsigned int *in[3], *out[3];
	char strFormat[4], strHash[2], strName[30];
	unsigned int maxColor;
	unsigned int imageHeight, imageWidth;
	unsigned int *red, *green, *blue, *sRed, *sGreen, *sBlue;
	
	int i, p1;

	/*DEVICE variables*/
	unsigned int *dev_in, *dev_out;

	/*Reading image header*/
	scanf("%s", strFormat); /*PPM format*/
	scanf("%s", strHash); /*hash*/
	scanf("%s", strName); /*name*/
	scanf("%u", &imageWidth);
	scanf("%u", &imageHeight);
	scanf("%u", &maxColor);

	/*Size of image array*/
	unsigned int size = imageHeight*imageWidth*sizeof(unsigned int);

	/*Allocating HOST memory*/
	for(i = 0; i < 3; i++)
	{
		//in[i] = (unsigned int*) malloc (size);
		//out[i] = (unsigned int*) malloc (size);
	}

	red = (unsigned int*) malloc (size);
	green = (unsigned int*) malloc (size);
	blue = (unsigned int*) malloc (size);
	sRed = (unsigned int*) malloc (size);
	sGreen = (unsigned int*) malloc (size);
	sBlue = (unsigned int*) malloc (size);

	/*Allocating DEVICE memory*/
	cudaMalloc( (void **) &dev_in, size);
	cudaMalloc( (void **) &dev_out, size);

	/*Reading image file*/
	for(i = 0; i < imageHeight*imageWidth; i++)
	{
		scanf("%d", &p1);
		red[i] = p1;
		scanf("%d", &p1);
		green[i] = p1;
		scanf("%d", &p1);
		blue[i] = p1;
	}

	int count = 0;
	if(_DEBUG_){
		printf("image:\n");
		for(i = 0; i < imageHeight*imageWidth; i++)
		{
			if(count%4 == 0)
				printf("\n");
			printf("%d ", red[i]);
			printf("%d ", green[i]);
			printf("%d ", blue[i]);
			count++;
			
		}
		printf("\n");
	}

	/*Defining blocks and grid dimensions*/
	/*1024 threads per block*/
	dim3 blockDim(1, 1); 
	dim3 gridDim(ceil(imageWidth/THREADS_PER_BLOCK), ceil(imageHeight/THREADS_PER_BLOCK));
	int dim = (imageWidth*imageHeight)/THREADS_PER_BLOCK;

	//for(i = 0; i < 3; i++)
	//{
		//if(_DEBUG_) printf("Color :%d\n", i);
		/*Copy input to device*/
		//cudaMemcpy(dev_in, in[0], size, cudaMemcpyHostToDevice);
		cudaMemcpy(dev_in, red, size, cudaMemcpyHostToDevice);
		/*The smooth kernel will run with numOfPixels/threadsPerBlock blocks and 1024 threads per block*/
	    smooth<<<gridDim,1>>>(dev_in, dev_out, imageHeight, imageWidth, maxColor);
	    /*Copy the result from DEVICE to HOST*/
	    //cudaMemcpy(out[0], dev_out, size, cudaMemcpyDeviceToHost);
	    cudaMemcpy(sRed, dev_out, size, cudaMemcpyDeviceToHost);
		
		cudaMemcpy(dev_in, green, size, cudaMemcpyHostToDevice);
		//cudaMemcpy(dev_in, in[1], size, cudaMemcpyHostToDevice);
	    smooth<<<gridDim,1>>>(dev_in, dev_out, imageHeight, imageWidth, maxColor);
	    /*Copy the result from DEVICE to HOST*/
	    //cudaMemcpy(out[1], dev_out, size, cudaMemcpyDeviceToHost);
	    cudaMemcpy(sGreen, dev_out, size, cudaMemcpyDeviceToHost);
	    
	    cudaMemcpy(dev_in, blue, size, cudaMemcpyHostToDevice);
	    //cudaMemcpy(dev_in, in[2], size, cudaMemcpyHostToDevice);
	    smooth<<<gridDim,1>>>(dev_in, dev_out, imageHeight, imageWidth, maxColor);
	    /*Copy the result from DEVICE to HOST*/
	    //cudaMemcpy(out[2], dev_out, size, cudaMemcpyDeviceToHost);
	    cudaMemcpy(sBlue, dev_out, size, cudaMemcpyDeviceToHost);
	    
	//}
	
    /*
	 *Creates and writes in a new image file
	 */
	FILE *smoothedImage;
	smoothedImage = fopen("out.ppm", "w");
	
	int j;
	fprintf(smoothedImage, "%s\n%s %s\n%u %u\n%u\n", strFormat, strHash, strName, imageWidth, imageHeight, maxColor);
	for(j = 0; j < imageWidth*imageHeight; j++)
	{
		if(count%4 == 0)
			fprintf(smoothedImage, "\n", NULL);
		fprintf(smoothedImage, "%d ",sRed[j]);
		fprintf(smoothedImage, "%d ",sGreen[j]);
		fprintf(smoothedImage, "%d ",sBlue[j]);
		count++;
	}
	
	if(0){	
		printf("smoothed image:\n");
		for(i = 0; i < imageHeight*imageWidth; i++)
		{
			if(count%4 == 0)
				printf("\n");
			printf("%d ", sRed[i]);
			printf("%d ", sGreen[i]);
			printf("%d ", sBlue[i]);
			count++;
		}
		printf("\n");
	}

    return 0;
}