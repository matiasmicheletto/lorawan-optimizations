#!/bin/bash


#inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat" # Instance
inputfile="../dat/generada.dat" # Instance
#inputfile="../dat/imu_100_10_100_1.dat" # Instance


resultfile="pareto_MO" # Result file

# Crossover methods: "UNIFORM" "SINGLE_POINT" "DOUBLE_POINT"
cr_method="SINGLE_POINT"

greedy_iters=10 # Number of iterations for Greedy
moga2_iters=10 # Number of iterations for MOGA2
moga2_pop=20 # GA population

# check if file exists
if [ ! -f $inputfile ]; then
    echo "File $inputfile not found!"
    exit 1
fi

# Create file if not exists and clear
touch $resultfile

# Clear file
> $resultfile

o_values=("GW" "E" "UF") # Objective parameter to optimize for MOGA2

# Run all
for a in 0.0 0.5 1; do
    for b in 0.0 0.5 1; do
        for g in 0.0 0.5 1; do
            echo "Running instance for tunning: alpha=$a, beta=$b, gamma=$g ..."  
            for c in 0.2 0.4 0.6 0.8; do
                for m in 0.2 0.4 0.6 0.8; do
                    for i in "${!o_values[@]}"; do
                        echo "Param values: cr=$c, mut=$m, obj=${o_values[i]}, no custom crossover"
                        echo -n "$a,$b,$g,$c,$m,no_custom_c," >> $resultfile.csv
                        ../bin/greedy -a $a -b $b -g $g -f $inputfile -i $greedy_iters -p $moga2_pop | ../bin/moga2 -a $a -b $b -g $g -f $inputfile -l $cr_method -z "${o_values[i]}" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV >> $resultfile.csv

                        # echo "Param values: cr=$c, mut=$m, obj=${o_values[i]}, using custom crossover"
                        #echo -n "$a,$b,$g,$c,$m,custom_c" >> $resultfile.csv
                        #../bin/greedy -a $a -b $b -g $g -f $inputfile -i $greedy_iters -p $moga2_pop | ../bin/moga2 -a $a -b $b -g $g -f $inputfile -l $cr_method -z "${o_values[i]}" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV >> $resultfile.csv
                    done
                done 
            done
        done
    done
done

    
echo "Results saved in $resultfile.csv"

# Activate virtual environment
source ../python/venv/bin/activate

# Run python script to plot the results, save figure and print text to same file name with .txt extension
python3 ../python/plotter-pareto-mo.py $resultfile >> $resultfile.txt

# Deactivate virtual environment
deactivate