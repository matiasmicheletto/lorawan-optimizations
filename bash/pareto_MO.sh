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
touch $resultfile.csv
touch $resultfile.txt

# Clear files
> $resultfile.csv
> $resultfile.txt

o_values=("GW" "E" "UF") # Objective parameter to optimize for MOGA2

# Run all
for a in 0.0 0.5 1; do
    for b in 0.0 0.5 1; do
        for g in 0.0 0.5 1; do
            echo "Running instance for tunning: alpha=$a, beta=$b, gamma=$g ..."  
            for c in 0.2 0.4 0.6 0.8; do
                for m in 0.2 0.4 0.6 0.8; do
                    for i in "${!o_values[@]}"; do

                        # Run and save execution output for generic crossover
                        echo "Param values: cr=$c, mut=$m, obj=${o_values[i]}, generic crossover"
                        output1=../bin/greedy -a $a -b $b -g $g -f $inputfile -i $greedy_iters -p $moga2_pop | ../bin/moga2 -a $a -b $b -g $g -f $inputfile -l $cr_method -z "${o_values[i]}" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV 2>/dev/null
                        exit_status1=$? # Error of last command

                        # Run and save execution output for custom crossover
                        #echo "Param values: cr=$c, mut=$m, obj=${o_values[i]}, custom crossover"
                        #output2=../bin/greedy -a $a -b $b -g $g -f $inputfile -i $greedy_iters -p $moga2_pop | ../bin/moga2 -a $a -b $b -g $g -f $inputfile -l $cr_method -z "${o_values[i]}" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV >> $resultfile.csv
                        #exit_status2=$? # Error of last command

                        if [ $exit_status1 -eq 0 ]; then
                            echo -n "$a,$b,$g,$c,$m,generic_cross," >> $resultfile.csv
                            echo $output1 >> $resultfile.csv
                        else
                            echo "Execution failed for $a,$b,$g,$c,$m,generic crossover"
                        fi

                        #if [ $exit_status2 -eq 0 ]; then
                        #    echo -n "$a,$b,$g,$c,$m,$o_values[i],custom_cross" >> $resultfile.csv
                        #    echo $output2 >> $resultfile.csv
                        #else
                        #    echo "Execution failed for $a,$b,$g,$c,$m,custom crossover"
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