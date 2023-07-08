#!/bin/bash

# Clear the output log before starting
> ../dat/output_ihu100_10_100.txt

# Optimization parameters
a_values=(1 1 10 1 10 1)
b_values=(0.01 0.1 0.01 0.01 0.01 0.1)
g_values=(1 1 1 100 100 100)
m_values=("GGW" "GE" "GU" "IRS" "GA")

for i in "${!a_values[@]}"; do
  for j in "${!m_values[@]}"; do
    ../bin/gpprs -v \
      -a "${a_values[i]}" \
      -b "${b_values[i]}" \
      -g "${g_values[i]}" \
      -m "${m_values[j]}" \
      -f ../dat/ihu100_10_100.dat >> ../dat/output_ihu100_10_100.txt &

    pid=$!

    # Wait for the process to finish before starting the next one
    wait "$pid"
  done
done &
