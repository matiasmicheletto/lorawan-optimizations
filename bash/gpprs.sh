#!/bin/bash

a_values=(1 1 1)
b_values=(0.01 1 0.01)
g_values=(1 1 1000)

m_values=("G2")

files=(
"../dat/imu_100_10_100_1.dat"
"../dat/imu_100_10_100_2.dat"
"../dat/imu_100_10_100_3.dat"
"../dat/imu_100_10_100_4.dat"
"../dat/imu_100_10_100_5.dat"
"../dat/imu_100_10_100_1.dat"
"../dat/imu_100_10_100_2.dat"
"../dat/imu_100_10_100_3.dat"
"../dat/imu_100_10_100_4.dat"
"../dat/imu_100_10_100_5.dat"
"../dat/imu_1000_20_100_1.dat"
"../dat/imu_1000_20_100_2.dat"
"../dat/imu_1000_20_100_3.dat"
"../dat/imu_1000_20_100_4.dat"
"../dat/imu_1000_20_100_5.dat"
"../dat/imu_1000_20_100_1.dat"
"../dat/imu_1000_20_100_2.dat"
"../dat/imu_1000_20_100_3.dat"
"../dat/imu_1000_20_100_4.dat"
"../dat/imu_1000_20_100_5.dat"
)

for k in "${!m_values[@]}"; do
  for j in "${!files[@]}"; do
    for i in "${!a_values[@]}"; do
      ../bin/gpprs -v \
          -a "${a_values[i]}" \
          -b "${b_values[i]}" \
          -g "${g_values[i]}" \
          -m "${m_values[k]}" \
          -f "${files[j]}" &
      pid=$!

      wait "$pid"
    done
  done
done &
