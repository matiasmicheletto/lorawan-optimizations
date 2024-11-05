#!/bin/bash

# Instances:
inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat"
#inputfile="../dat/instancia10_5.dat"

# Result file
resultfile="greedy_tunning_sweep.csv"

# Repetitions
reps=25

# Create file if not exists and clear
touch $resultfile

# Clear file
> $resultfile

# Run all
# for a, b, c in 0..1
for k in $(seq 1 $reps);do
    echo "Starting iteration $k"
    for a in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.25 1); do
        for b in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.25 1); do
            for g in $(LC_NUMERIC="en_US.UTF-8" seq 0 0.25 1); do
                echo -n "$a,$b,$g," >> $resultfile
                ../bin/greedy -f $inputfile -a $a -b $b -g $g -i 100 -x >> $resultfile
            done
        done
    done
done
echo "Results saved in $resultfile"

# Activate virtual environment
source ../python/venv/bin/activate

# Run python script to plot the results
python3 ../python/pareto-plotter2.py $resultfile

# Deactivate virtual environment
deactivate