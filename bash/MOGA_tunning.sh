#!/bin/bash


#inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat" # Instance
#inputfile="../dat/generada.dat" # Instance
#inputfile="../dat/imu_100_10_100_1.dat" # Instance
inputfile="../dat/instancia.dat" # Instance


resultfile="MOGA_tunning" # Result file

# Crossover methods: "UNIFORM" "SINGLE_POINT" "DOUBLE_POINT"
cr_method="SINGLE_POINT"

greedy_iters=10 # Number of iterations for Greedy
moga2_iters=10 # Number of iterations for MOGA2
moga2_pop=20 # GA population

# check if file exists
if [ ! -f "$inputfile" ]; then
    echo "File $inputfile not found!"
    exit 1
fi

# Create file if not exists and clear
touch $resultfile.csv

# Clear files
> $resultfile.csv

o_values=("GW" "E" "UF") # Objective parameter to optimize for MOGA2

# Run greedy algorithm and save solution
../bin/greedy -f "$inputfile" -i $greedy_iters -p $moga2_pop > greedysol.sol

# Run MOGA2 with different parameters
for c in 0.3 0.5 0.7; do
    for m in 0.05 0.1 0.15 0.2; do
        for param in "${!o_values[@]}"; do

            obj_value="${o_values[param]}" # GW, E, UF

            # Run and save execution output for generic crossover
            echo "Param values: cr=$c, mut=$m, obj=$obj_value, generic crossover"

            output1=$(cat greedysol.sol | ../bin/moga2 -f "$inputfile" -l "$cr_method" -z "$obj_value" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV 2>/dev/null)
            exit_status1=$? # Error of last command

            if [[ $exit_status1 -eq 0 ]]; then
                echo -n "$c,$m,generic_cross," >> $resultfile.csv
                echo $output1 >> $resultfile.csv
            else
                echo "Execution failed for: cr=$c, mut=$m, obj=$obj_value, generic crossover"
            fi

            
            # Run and save execution output for custom crossover
            #echo "Param values: cr=$c, mut=$m, obj=$obj_value, custom crossover"
            
            #output2=$(cat greedy.sol | ../bin/moga2 -f "$inputfile" -l "$cr_method" -z "$obj_value" -c $c -m $m -i $moga2_iters -p -s 1 -x CSV 2>/dev/null)
            #exit_status2=$? # Error of last command
            
            #if [[ $exit_status2 -eq 0 ]]; then
            #    echo -n "$c,$m,$o_values[i],custom_cross" >> $resultfile.csv
            #    echo $output2 >> $resultfile.csv
            #else
            #    echo "Execution failed for: cr=$c, mut=$m, obj=$obj_value, custom crossover"
        done
    done 
done

    
echo "Results saved in $resultfile.csv"
