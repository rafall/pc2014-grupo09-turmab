size="1000"
file="out.out"

montecarlo_seq(){
    path="montecarlo/sequencial"
    
    make --silent --directory=$path clean 
    make --silent --directory=$path

    echo "Montecarlo Sequencial\n"
    echo "$size" > $path/$file
    #for i in $(seq $size); do 
        #/usr/bin/time -f "%e" ./$path/montecarlo >> $path/$file 2>&1
    #done
    
    make --silent --directory=$path clean
}

montecarlo_par(){
    path="montecarlo/paralelo"
    
    make --silent --directory=$path clean 
    make --silent --directory=$path
    
    echo "Montecarlo Paralelo\n"
    echo "$size" > $path/$file
    #for i in $(seq $size); do 
    #    /usr/bin/time -f "%e" ./$path/montecarlo > $path/$file
    #done
    
    make --silent --directory=$path clean
}

blackscholes_seq(){
    path="blackscholes/sequencial"
    
    make --silent --directory=$path clean 
    make --silent --directory=$path
    
    echo "blackscholes Sequencial\n"
    echo "$size" > $path/$file
    #for i in $(seq $size); do 
    #    /usr/bin/time -f "%e" ./$path/blackscholes > $path/$file
    #done
    
    make --silent --directory=$path clean
}

blackscholes_par(){
    path="blackscholes/paralelo"
    
    make --silent --directory=$path clean 
    make --silent --directory=$path
    
    echo "blackscholes Paralelo\n"
    echo "$size" > $path/$file
    #for i in $(seq $size); do 
    #    /usr/bin/time -f "%e" ./$path/blackscholes > $path/$file
    #done
    
    make --silent --directory=$path clean
}

main(){
    montecarlo_seq

    montecarlo_par
    
    blackscholes_seq
    
    blackscholes_par
}

main
