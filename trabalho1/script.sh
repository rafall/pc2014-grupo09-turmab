#!/bin/bash

size="2"
file="out.out"
threads="4 8 12 16 32"

method="Montecarlo Sequencial"
directory="montecarlo/sequencial"

montecarlo_seq(){
	path="montecarlo/sequencial"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo "Montecarlo Sequencial\n"
	echo "$size 2" > $path/$file
	for i in $(seq $size); do 
		/usr/bin/time -f "%e" ./$path/montecarlo >> $path/$file 2>&1
	done

	make --silent --directory=$path clean
}

blackscholes_seq(){
	path="blackscholes/sequencial"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo "Blackscholes Sequencial\n"
	echo "$size 3" > $path/$file
	for i in $(seq $size); do 
		/usr/bin/time -f "%e" ./$path/blackscholes < $path/entrada_blackscholes.txt >> $path/$file 2>&1
	done

	make --silent --directory=$path clean
}

montecarlo_par(){
	path="montecarlo/paralelo"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo "Montecarlo Paralelo\n"

	for j in $threads; do
		echo "$size 2" > $path/$j$file 
		for i in $(seq $size); do
			/usr/bin/time -f "%e" ./$path/montecarlo $j >> $path/$j$file 2>&1
		done
	done

	make --silent --directory=$path clean
}

blackscholes_par(){
	path="blackscholes/paralelo"

	make --silent --directory=$path clean 
	make --silent --directory=$path

	echo "Blackscholes Paralelo\n"

	for j in $threads; do
		echo "$size 3" > $path/$j$file 
		for i in $(seq $size); do 
			/usr/bin/time -f "%e" ./$path/blackscholes $j < $path/entrada_blackscholes.txt >> $path/$j$file 2>&1
		done
	done

	make --silent --directory=$path clean
}

mean_variance_seq(){
	path="mean_var"
	mean_file="mean_var.out"

	echo "Calculando media e variancia para $method\n"
	./$path/mean_var < $directory/$file > $directory/$mean_file
}

mean_variance_par(){
	path="mean_var"
	mean_file="mean_var.out"

	echo "Calculando media e variancia para $method\n"
	for i in $threads; do
		./$path/mean_var < $directory/$i$file > $directory/$i$mean_file
	done
}

main(){
	clear
	path="mean_var"

	make --silent --directory=$path clean 
	make --silent --directory=$path

#	montecarlo_seq
#	mean_variance_seq

#	montecarlo_par
#	method="Montecarlo Paralelo"
#	directory="montecarlo/paralelo"
#	mean_variance_par

	blackscholes_seq
	method="Blackscholes Sequencial"
	directory="blackscholes/sequencial"
	mean_variance_seq

	blackscholes_par
	method="Blackscholes Paralelo"
	directory="blackscholes/paralelo"
	mean_variance_par

	make --silent --directory=$path clean
}

main
