import os
from utils import *


input_file_name = '../results/all.csv'
output_file_name = 'tables.tex'

# Remove latex file before starting
os.remove(output_file_name)

# Load dataframe from file and prepare columns
data = init_dataframe(input_file_name)

# Build each table averaging values
df_heu_u_s_s = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'SI')
df_cplex_u_s_s = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'SI')

df_heu_u_s_n = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'NO')
df_cplex_u_s_n = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'NO')

df_heu_u_m = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM')
df_cplex_u_m = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM')

df_heu_u_h = average_group(data, 'HEU', 'UNIFORM', 'HARD')
df_cplex_u_h = average_group(data, 'CPLEX', 'UNIFORM', 'HARD')

# Write tables
to_latex(df_heu_u_s_s, df_cplex_u_s_s, "Uniform distribution, soft real time constraints with essential gateways", "tab:unif_soft_esc", output_file_name)
to_latex(df_heu_u_s_n, df_cplex_u_s_n, "Uniform distribution, soft real time constraints without essential gateways", "tab:unif_soft_n_esc", output_file_name)
to_latex(df_heu_u_m, df_cplex_u_m, "Uniform distribution, medium real time constraints", "tab:unif_m", output_file_name)
to_latex(df_heu_u_h, df_cplex_u_h, "Uniform distribution, medium real time constraints", "tab:unif_m", output_file_name)