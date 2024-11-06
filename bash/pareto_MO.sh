#!/bin/bash

# Instances:
inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat"

# check if file exists
if [ ! -f $inputfile ]; then
    echo "File $inputfile not found!"
    exit 1
fi

# Result file
resultfile="pareto_MO.csv"

# Objective parameter to optimize for MOGA2
o_values=("GW" "E" "UF")

# Create file if not exists and clear
touch $resultfile

# Clear file
> $resultfile

# Run all
for a in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.5 1); do
    for b in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.5 1); do
        for g in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.5 1); do
            echo "Running instance for $a,$b,$g ..."  
            for i in "${!o_values[@]}"; do
                echo -n "$a,$b,$g," >> $resultfile
                ../bin/greedy -a $a -b $b -g $g -f $inputfile -i 100 -p 30 | ../bin/moga2 -f $inputfile -z "${o_values[i]}" -i 100 -p -s 1 -x CSV >> $resultfile
            done
        done
    done
done
    
echo "Results saved in $resultfile"

# Activate virtual environment
source ../python/venv/bin/activate

# Run python script to plot the results, save figure and print text to same file name with .txt extension
python3 ../python/plotter-pareto-mo.py $resultfile >> $resultfile.txt

# Deactivate virtual environment
deactivate