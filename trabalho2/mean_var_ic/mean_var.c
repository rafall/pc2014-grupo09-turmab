/*Todo - add the confidence interval and comment the code properly*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double mean(double *values, unsigned int size){
	if(size != 0){
		double mediaCalc = 0.0;
		unsigned int i = 0;
		for(;i<size;i++)
			mediaCalc += values[i];
		return mediaCalc/size;
	}
	return 0.0;
}

double variance(double *values, double mean, unsigned int size){
	if(size != 1){
		double variancia1Calc = 0.0f;
		unsigned int i = 0;
		for(;i<size;i++){
			variancia1Calc += pow(values[i]-mean,2.0f);
		}
		return variancia1Calc/(double)(size-1);
	}
	return -1;
}

double confidence_interval(double *values, ){
}

int main(void){
	unsigned long int n = 0, n_var = 0;
	unsigned int i = 0, j =0;
	double **data;
/*	float mediaCalculada;*/

	scanf("%lu %lu",&n, &n_var);

	data = (double **)malloc(n_var*sizeof(double*));

	if(NULL == data)
		return EXIT_FAILURE;

	for(i = 0; i < n_var; i++){
		data[i] = (double*)malloc(n*(sizeof(double)));
		if(NULL == data[i])
			return EXIT_FAILURE;
	}

	for(i = 0; i < n; i++){
		for( j = 0; j < n_var; j++){
			scanf("%lf", &data[j][i]);
		}
	}

	for(i = 0; i < n_var; i++){
		double dmean = mean(data[i],n);
		double var = variance(data[i], dmean, n);
		printf("%lf\t%lf\n", dmean, var);
	}

	for(i = 0; i < n_var; i++)
		free(data[i]);

	free(data);

	return EXIT_SUCCESS;
}
