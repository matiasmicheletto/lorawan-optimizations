#!/usr/bin/env bash

#inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat" # Instance
#inputfile="../dat/imu_100_10_100_1.dat" # Instance
inputfile="../dat/instancia.dat" # Instance
outputfile="promediar.csv" # Result file

# check if file exists
if [ ! -f "$inputfile" ]; then
    echo "File $inputfile not found!"
    exit 1
fi

> $outputfile

# Run 1000 times
for ((i=1; i<=100; i++)); do
    output=$(../bin/greedy -f "$inputfile" -i 1000 -x)
    exit_status=$?
    if [[ $exit_status -eq 0 ]]; then
        echo "$output" >> "$outputfile"
    else
        echo "Error executing greedy algorithm"
        exit 1
    fi
    echo "Progress: $((i * 100 / 100))%"
done

echo "Execution completed. Results saved in $outputfile"