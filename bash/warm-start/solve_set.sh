
rm summary.csv

# GA - random
./ga -f s_u_100_10_1000.dat -i 100 -t 60
./ga -f s_u_500_10_1000.dat -i 100 -t 60
./ga -f s_u_1000_10_1000.dat -i 100 -t 60
./ga -f s_u_1500_20_1000.dat -i 100 -t 60

./ga -f m_u_100_10_1000.dat -i 100 -t 60
./ga -f m_u_500_10_1000.dat -i 100 -t 60
./ga -f m_u_1000_10_1000.dat -i 100 -t 60
./ga -f m_u_1500_20_1000.dat -i 100 -t 60

./ga -f h_u_100_10_1000.dat -i 100 -t 60
./ga -f h_u_500_10_1000.dat -i 100 -t 60
./ga -f h_u_1000_10_1000.dat -i 100 -t 60
./ga -f h_u_1500_10_1000.dat -i 100 -t 60

./ga -f m_n_100_10_1000.dat -i 100 -t 60
./ga -f m_n_500_10_1000.dat -i 100 -t 60
./ga -f m_n_1000_10_1000.dat -i 100 -t 60
./ga -f m_n_1500_10_1000.dat -i 100 -t 60

./ga -f h_n_100_10_1000.dat -i 100 -t 60
./ga -f h_n_500_10_1000.dat -i 100 -t 60
./ga -f h_n_1000_10_1000.dat -i 100 -t 60
./ga -f h_n_1500_10_1000.dat -i 100 -t 60


# GA warmstart
./greedy -f s_u_100_10_1000.dat -i 50 -p 20 | ./ga -f s_u_100_10_1000.dat -p -i 100 -t 60
./greedy -f s_u_500_10_1000.dat -i 50 -p 20 | ./ga -f s_u_500_10_1000.dat -p -i 100 -t 60
./greedy -f s_u_1000_10_1000.dat -i 50 -p 20 | ./ga -f s_u_1000_10_1000.dat -p -i 100 -t 60
./greedy -f s_u_1500_20_1000.dat -i 50 -p 20 | ./ga -f s_u_1500_20_1000.dat -p -i 100 -t 60

./greedy -f m_u_100_10_1000.dat -i 50 -p 20 | ./ga -f m_u_100_10_1000.dat -p -i 100 -t 60
./greedy -f m_u_500_10_1000.dat -i 50 -p 20 | ./ga -f m_u_500_10_1000.dat -p -i 100 -t 60
./greedy -f m_u_1000_10_1000.dat -i 50 -p 20 | ./ga -f m_u_1000_10_1000.dat -p -i 100 -t 60
./greedy -f m_u_1500_20_1000.dat -i 50 -p 20 | ./ga -f m_u_1500_20_1000.dat -p -i 100 -t 60

./greedy -f h_u_100_10_1000.dat -i 50 -p 20 | ./ga -f h_u_100_10_1000.dat -p -i 100 -t 60
./greedy -f h_u_500_10_1000.dat -i 50 -p 20 | ./ga -f h_u_500_10_1000.dat -p -i 100 -t 60
./greedy -f h_u_1000_10_1000.dat -i 50 -p 20 | ./ga -f h_u_1000_10_1000.dat -p -i 100 -t 60
./greedy -f h_u_1500_10_1000.dat -i 50 -p 20 | ./ga -f h_u_1500_10_1000.dat -p -i 100 -t 60

./greedy -f m_n_100_10_1000.dat -i 50 -p 20 | ./ga -f m_n_100_10_1000.dat -p -i 100 -t 60
./greedy -f m_n_500_10_1000.dat -i 50 -p 20 | ./ga -f m_n_500_10_1000.dat -p -i 100 -t 60
./greedy -f m_n_1000_10_1000.dat -i 50 -p 20 | ./ga -f m_n_1000_10_1000.dat -p -i 100 -t 60
./greedy -f m_n_1500_10_1000.dat -i 50 -p 20 | ./ga -f m_n_1500_10_1000.dat -p -i 100 -t 60

./greedy -f h_n_100_10_1000.dat -i 50 -p 20 | ./ga -f h_n_100_10_1000.dat -p -i 100 -t 60
./greedy -f h_n_500_10_1000.dat -i 50 -p 20 | ./ga -f h_n_500_10_1000.dat -p -i 100 -t 60
./greedy -f h_n_1000_10_1000.dat -i 50 -p 20 | ./ga -f h_n_1000_10_1000.dat -p -i 100 -t 60
./greedy -f h_n_1500_10_1000.dat -i 50 -p 20 | ./ga -f h_n_1500_10_1000.dat -p -i 100 -t 60


