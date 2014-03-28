#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float media(float *values, unsigned int size);
float variancia1(float *values, float media, unsigned int size);

int main(void){
	unsigned long int n = 0;
	unsigned int i = 0;
	float *vectorFloats;
	float mediaCalculada;
	do{
		scanf("%lu",&n);
	}while(n < 1);
	vectorFloats = (float*)malloc(n*(sizeof(float)));
	if(vectorFloats == NULL)
		exit(EXIT_FAILURE);
	for(;i<n;i++)
		scanf("%f",	&vectorFloats[i]);
	mediaCalculada = media(vectorFloats,n);
	printf("Valor da Média: %f\n", mediaCalculada);
	printf("Valor da Variacia pelo metodo: %f\n", variancia1(vectorFloats, mediaCalculada, n));
	free(vectorFloats);
	return 0;
}

float media(float *values, unsigned int size){
	if(size != 0){
		float mediaCalc = 0.0f;
		unsigned int i = 0;
		for(;i<size;i++)
			mediaCalc = mediaCalc + values[i];
		return mediaCalc/size;
	}
	return 0.0;
}

float variancia1(float *values, float media, unsigned int size){
	if(size != 1){
		float variancia1Calc = 0.0f;
		unsigned int i = 0;
		for(;i<size;i++){
			variancia1Calc = variancia1Calc + pow(values[i]-media,2.0f);
		}
		return variancia1Calc/(float)(size-1);
	}
	return -1;
}
