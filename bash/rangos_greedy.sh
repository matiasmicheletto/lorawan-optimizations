#!/bin/bash

inputfile="../dat/Revista/ComparacionModelosHeuristicaHard/generada.dat" # Instance
outputfile="promediar.csv" # Result file

# check if file exists
if [ ! -f "$inputfile" ]; then
    echo "File $inputfile not found!"
    exit 1
fi

touch $outputfile.csv

> $outputfile.csv

# Run 1000 times
for i in {1..100}
    output=$(../bin/greedy -f "$inputfile" -i 1000 -x)
    exit_status=$? # Error of last command
    if [[ $exit_status -eq 0 ]]; then
        echo $output >> $outputfile.csv
    else
        echo "Error executing greedy algorithm"
        exit 1
    fi
done

echo "Execution completed. Results saved in $outputfile.csv"