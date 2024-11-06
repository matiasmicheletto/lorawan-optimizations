#!/bin/bash

# Instances:
inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat"

# Result file
resultfile="MO_results.csv"

# Objective parameter to optimize for MOGA2
o_values=("GW" "E" "UF")

# Repetitions
reps=20

# Create file if not exists and clear
touch $resultfile

# Clear file
> $resultfile

# Run all
for k in $(seq 1 $reps); do
    echo "Starting iteration $k"
    (flock -x 200; ../bin/greedymo -f $inputfile -x >> $resultfile) 200>$resultfile.lock &
    echo "Greedy done."
    for i in "${!o_values[@]}"; do
        # Run optimizer for inputfile using (GW, E, UF) as objective and export results to resultfile
        (flock -x 200; ../bin/moga2 -f $inputfile -z "${o_values[i]}" -i 50 -q 20 -s 1 -x CSV >> $resultfile) 200>$resultfile.lock &
        echo "MOGA2 done."
        (flock -x 200; ../bin/greedy -f $inputfile -p 20 | ../bin/moga2 -f $inputfile -z "${o_values[i]}" -i 50 -p -s 1 -x CSV >> $resultfile) 200>$resultfile.lock &
        echo "MOGA2+WS done."
    done
    #pid=$!
    #wait "$pid"
    wait
    echo "Iteration $k finished."
done
echo "Results saved in $resultfile"


# Activate virtual environment
source ../python/venv/bin/activate

# Run python script to plot the results
python3 ../python/plotter-mo-plus-greedy.py $resultfile

# Deactivate virtual environment
deactivate