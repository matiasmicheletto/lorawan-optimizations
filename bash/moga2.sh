#!/bin/bash

# Instances:
inputfile="../dat/imu_100_10_100_5.dat"

# Result file
resultfile="moga2.csv"

# Objective parameter to optimize
o_values=("GW" "E" "UF")

# Repetitions
reps=3

# Create file if not exists and clear
touch $resultfile
# Clear file
> $resultfile

# Run MOGA2
for i in "${!o_values[@]}"; do
    for k in $(seq 1 $reps); do
        # Run optimizer for inputfile using (GW, E, UF) as objective and export results to resultfile
        ../bin/moga2 -f $inputfile -z "${o_values[i]}" -i 50 -q 20 -s 1 -x CSV >> $resultfile &
        echo "Solving $inputfile for objective ${o_values[i]}"
        pid=$!
        wait "$pid"
    done
done
echo "Results saved in ${resultfile}"


# Activate virtual environment
source ../python/venv/bin/activate

# Run python script to plot the results
python3 ../python/pareto-plotter.py $resultfile

# Deactivate virtual environment
deactivate