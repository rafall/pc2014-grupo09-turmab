montecarlo_seq(){
    make -C montecarlo/sequencial clean 
    make -C montecarlo/sequencial
    clear
    echo "Montecarlo Sequencial\n".
    ./montecarlo/sequencial/montecarlo
    sleep(2)
    clear
    make -C montecarlo/sequencial clean
}

montecarlo_par(){
    make -C montecarlo/paralelo clean 
    make -C montecarlo/paralelo
    clear
    echo "Montecarlo Paralelo\n".
    ./montecarlo/paralelo/montecarlo
    sleep(2)
    clear
    make -C montecarlo/paralelo clean
}

main(){
    clear
    montecarlo_seq
    montecarlo_par
}

main
