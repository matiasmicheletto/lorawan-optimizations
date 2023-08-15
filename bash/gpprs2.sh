#!/bin/bash

a_values=(1 1 1 10 100 1 1 1 10 10 10 1 1 1)
b_values=(0.01 0.1 1 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.01 0.1 0.1 0.1)
g_values=(1 1 1 1 1 10 100 1000 10 100 1000 10 100 1000)

m_values=("G2" "IRS")

files=("../dat/last/ihu10000_100_500.dat" 
"../dat/last/ihu2000_10_100.dat" 
"../dat/last/imu100_10_100.dat" 
"../dat/last/isu10000_100_500.dat" 
"../dat/last/isu20000_30_500.dat" 
"../dat/last/ihu10000_20_100.dat" 
"../dat/last/ihu5000_100_500.dat" 
"../dat/last/imu100_30_500.dat" 
"../dat/last/isu10000_20_100.dat" 
"../dat/last/isu2000_100_500.dat" 
"../dat/last/ihu1000_100_500.dat" 
"../dat/last/ihu5000_20_100.dat" 
"../dat/last/imu20000_100_500.dat" 
"../dat/last/isu10000_30_500.dat" 
"../dat/last/isu2000_10_100.dat" 
"../dat/last/ihu1000_10_100.dat" 
"../dat/last/imu10000_100_500.dat" 
"../dat/last/imu20000_30_500.dat" 
"../dat/last/isu1000_100_500.dat" 
"../dat/last/isu2000_30_500.dat" 
"../dat/last/ihu1000_30_500.dat" 
"../dat/last/imu10000_20_100.dat" 
"../dat/last/imu2000_100_500.dat" 
"../dat/last/isu1000_10_100.dat" 
"../dat/last/isu5000_100_500.dat" 
"../dat/last/ihu100_100_500.dat" 
"../dat/last/imu10000_30_500.dat" 
"../dat/last/imu2000_10_100.dat" 
"../dat/last/isu1000_30_500.dat" 
"../dat/last/isu5000_20_100.dat" 
"../dat/last/ihu100_10_100.dat" 
"../dat/last/imu1000_100_500.dat" 
"../dat/last/imu2000_30_500.dat" 
"../dat/last/isu100_100_500.dat" 
"../dat/last/isu5000_30_500.dat" 
"../dat/last/ihu100_30_500.dat" 
"../dat/last/imu1000_10_100.dat" 
"../dat/last/imu5000_100_500.dat" 
"../dat/last/isu100_10_100.dat" 
"../dat/last/ihu20000_100_500.dat" 
"../dat/last/imu1000_30_500.dat" 
"../dat/last/imu5000_20_100.dat" 
"../dat/last/isu100_30_500.dat" 
"../dat/last/ihu2000_100_500.dat" 
"../dat/last/imu100_100_500.dat" 
"../dat/last/imu5000_30_500.dat" 
"../dat/last/isu20000_100_500.dat" 
)

for k in "${!m_values[@]}"; do
  for j in "${!files[@]}"; do
    for i in "${!a_values[@]}"; do
      ../bin/gpprs -v \
          -a "${a_values[i]}" \
          -b "${b_values[i]}" \
          -g "${g_values[i]}" \
          -m "${m_values[k]}" \
          -i 1000000 \
          -f "${files[j]}" &
      pid=$!

      wait "$pid"
    done
  done
done &
