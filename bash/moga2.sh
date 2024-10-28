#!/bin/bash

# Instances:
files=(
"../dat/imu_1000_20_100_2.dat"
"../dat/imu_100_10_100_5.dat"
)

# Objective parameter to optimize
o_values=("GW" "E" "UF")

# Repetitions
reps=5

# Create file if not exists
touch moga2.csv
# Clear file
> moga2.csv

# Run MOGA2
for i in "${!o_values[@]}"; do
    for j in "${!files[@]}"; do
        for k in $(seq 1 $reps); do
            ../bin/moga2 -f "${files[j]}" -z "${o_values[i]}" -i 10 -q 25 -s 1 -x CSV >> ../dat/moga2.sol &
            echo "Solving ${files[j]} for objective ${o_values[i]}"
            pid=$!
            wait "$pid"
        done
    done
done
echo "Done"