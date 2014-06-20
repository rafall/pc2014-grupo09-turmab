#!/bin/bash

size="20"
file="out"
filt_size="5 7 11"
images=("marvel.ppm")

seq_alg(){

	path="sequential"
	exe="smooth"
	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo -e "\t Make $path done."

	echo -e "\t $path algorithm running."
	for j in "${images[@]}"; do
		for m in $filt_size; do
			echo "$size 2" > $path/$m$j.$file
			for i in $(seq $size); do 
				echo "./$path/$exe $m < $j >> $path/$m$j 2> $path/$m$j.$file"
			done
		done
	done

	echo -e "\t $path algorithm done."

	make --silent --directory=$path clean 
}

seq_mean(){

	path="mean_var_ic"
	directory="sequential"
	exe="mean_var"
	mean_file="stat"
	make --silent --directory=$path clean
	make --silent --directory=$path

	echo -e "\t Make $path done."

	for j in "${images[@]}"; do
		for i in $filt_size; do
			echo "./$path/$exe < $directory/$i$j.$file > $directory/$i$j.$mean_file"
		done
	done

	echo -e "\t $path algorithm done."

	make --silent --directory=$path clean
}

par_alg(){

	path="parallel"
	exe="smooth"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo -e "\t Make $path done."

	echo -e "\t $path algorithm running."

	for j in "${images[@]}"; do
		for i in $filt_size; do
			echo "$size 2" > $path/$m$j.$file
			for m in $(seq $size); do
				./$path/$exe $i < $j > $path/$i$j 2> $path/$i$j.$file
			done
		done
	done

	echo -e "\t $path algorithm done."

	make --silent --directory=$path clean 
}

par_mean(){

	path="mean_var_ic"
	directory="parallel"
	exe="mean_var"
	mean_file="stat"

	make --silent --directory=$path clean
	make --silent --directory=$path

	echo -e "\t Make $path done."

	for j in "${images[@]}"; do
		for i in $filt_size; do
			./$path/$exe < $directory/$i$j.$file > $directory/$i$j.$mean_file
		done
	done

	echo -e "\t $path algorithm done."

	make --silent --directory=$path clean

}

main(){
	clear
	echo "Executing the script."

	echo "Executing the sequential algorithm."
		seq_alg
	echo "Execution of the sequential algorithm ended."

	echo "Calculating statistics for the sequential algorithm."
		seq_mean
	echo "Calculation of the statistics for the sequential algorithm done."

	echo "Executing the parallel algorithm."
		par_alg
	echo "Execution of the parallel algorithm ended."

	echo "Calculating the mean, variance, and confidence interval of the results for the parallel algorithm."
		par_mean
	echo "Calculation of the mean, variance, and confidence interval for the parallel algorithm done."

	echo "Script execution done."
}

main
