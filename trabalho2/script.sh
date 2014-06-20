#!/bin/bash

size="20"
file="out"
nodes="2 4 8 12 16"
tests=("input1" "input2" "input3")

seq_alg(){

	path="sequential"
	exe="icmctown"
	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo -e "\t Make $path done."

	echo -e "\t $path algorithm running."
	for j in "${tests[@]}"; do
		echo "$size 2" > $path/$j-$file
		for i in $(seq $size); do 
			/usr/bin/time -f "%e" ./$path/$exe < $path/$j >> $path/$j-$file 2>&1
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

	for j in "${tests[@]}"; do
		./$path/$exe < $directory/$j-$file > $directory/$j-$mean_file
	done

	echo -e "\t $path algorithm done."

	make --silent --directory=$path clean
}

par_alg(){

	path="parallel"
	exe="icmctown"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo -e "\t Make $path done."

	echo -e "\t $path algorithm running."

	for j in "${tests[@]}"; do
		for i in $nodes; do
			echo "$size 2" > $path/$j-$i$file
			for z in $(seq $size); do
				/usr/bin/time -f "%e" make run --silent --directory=$path FILE=$j CNP=$i >> $path/$j-$i$file 2>&1
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

	for j in "${tests[@]}"; do
		for i in $nodes; do
			./$path/$exe < $directory/$j-$i$file > $directory/$j-$i$mean_file
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
