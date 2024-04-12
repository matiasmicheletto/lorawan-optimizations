import os
from utils import *


input_file_name = '../results/all.csv'
output_file_name = 'tables.tex'

# Write first line
with open(output_file_name, 'w') as latexfile:  
    latexfile.write('\\section\{Results\} \\label\{appendix:tables\}\n\n')

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

df_heu_c_s_s = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'SI')
df_cplex_c_s_s = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'SI')

df_heu_c_s_n = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'NO')
df_cplex_c_s_n = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'NO')

df_heu_c_m = average_group(data, 'HEU', 'CLOUD', 'MEDIUM')
df_cplex_c_m = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM')

df_heu_c_h = average_group(data, 'HEU', 'CLOUD', 'HARD')
df_cplex_c_h = average_group(data, 'CPLEX', 'CLOUD', 'HARD')


# Write latex tables to output file
to_latex(df_heu_u_s_s, df_cplex_u_s_s, "Uniform distribution, soft real time constraints with essential gateways", "tab:unif_soft_esc", output_file_name)
to_latex(df_heu_u_s_n, df_cplex_u_s_n, "Uniform distribution, soft real time constraints without essential gateways", "tab:unif_soft_n_esc", output_file_name)
to_latex(df_heu_u_m, df_cplex_u_m, "Uniform distribution, medium real time constraints", "tab:unif_medium", output_file_name)
to_latex(df_heu_u_h, df_cplex_u_h, "Uniform distribution, hard real time constraints", "tab:unif_hard", output_file_name)

to_latex(df_heu_c_s_s, df_cplex_c_s_s, "Cloud distribution, soft real time constraints with essential gateways", "tab:cloud_soft_esc", output_file_name)
to_latex(df_heu_c_s_n, df_cplex_c_s_n, "Cloud distribution, soft real time constraints without essential gateways", "tab:cloud_soft_n_esc", output_file_name)
to_latex(df_heu_c_m, df_cplex_c_m, "Cloud distribution, medium real time constraints", "tab:cloud_medium", output_file_name)
to_latex(df_heu_c_h, df_cplex_c_h, "Cloud distribution, hard real time constraints", "tab:cloud_hard", output_file_name)