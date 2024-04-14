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
df_heu_u_s_n = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'NO')
df_heu_u_m_s = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'SI')
df_heu_u_m_n = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'NO')
df_heu_u_h_s = average_group(data, 'HEU', 'UNIFORM', 'HARD', 'SI')
df_heu_u_h_n = average_group(data, 'HEU', 'UNIFORM', 'HARD', 'NO')

df_heu_c_s_s = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'SI')
df_heu_c_s_n = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'NO')
df_heu_c_m_s = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'SI')
df_heu_c_m_n = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'NO')
df_heu_c_h_s = average_group(data, 'HEU', 'CLOUD', 'HARD', 'SI')
df_heu_c_h_n = average_group(data, 'HEU', 'CLOUD', 'HARD', 'NO')

df_cplex_u_s_s = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'SI')
df_cplex_u_s_n = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'NO')
df_cplex_u_m_s = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'SI')
df_cplex_u_m_n = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'NO')
df_cplex_u_h_s = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'SI')
df_cplex_u_h_n = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'NO')

df_cplex_c_s_s = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'SI')
df_cplex_c_s_n = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'NO')
df_cplex_c_m_s = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'SI')
df_cplex_c_m_n = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'NO')
df_cplex_c_h_s = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'SI')
df_cplex_c_h_n = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'NO')


# Write latex tables to output file
to_latex(df_heu_u_s_s, df_cplex_u_s_s, "Uniform distribution, soft real time constraints with essential gateways",      "tab:unif_soft_esc", output_file_name)
to_latex(df_heu_u_s_n, df_cplex_u_s_n, "Uniform distribution, soft real time constraints without essential gateways",   "tab:unif_soft_n_esc", output_file_name)
to_latex(df_heu_u_m_s, df_cplex_u_m_s, "Uniform distribution, medium real time constraints with essential gateways",    "tab:unif_medium_esc", output_file_name)
to_latex(df_heu_u_m_n, df_cplex_u_m_n, "Uniform distribution, medium real time constraints without essential gateways", "tab:unif_medium_n_esc", output_file_name)
to_latex(df_heu_u_h_s, df_cplex_u_h_s, "Uniform distribution, hard real time constraints with essential gateways",      "tab:unif_hard_esc", output_file_name)
to_latex(df_heu_u_h_n, df_cplex_u_h_n, "Uniform distribution, hard real time constraints without essential gateways",   "tab:unif_hard_n_esc", output_file_name)

to_latex(df_heu_c_s_s, df_cplex_c_s_s, "Cloud distribution, soft real time constraints with essential gateways",      "tab:cloud_soft_esc", output_file_name)
to_latex(df_heu_c_s_n, df_cplex_c_s_n, "Cloud distribution, soft real time constraints without essential gateways",   "tab:cloud_soft_n_esc", output_file_name)
to_latex(df_heu_c_m_s, df_cplex_c_m_s, "Cloud distribution, medium real time constraints with essential gateways",    "tab:cloud_medium_esc", output_file_name)
to_latex(df_heu_c_m_n, df_cplex_c_m_n, "Cloud distribution, medium real time constraints without essential gateways", "tab:cloud_medium_n_esc", output_file_name)
to_latex(df_heu_c_h_s, df_cplex_c_h_s, "Cloud distribution, hard real time constraints with essential gateways",      "tab:cloud_hard_esc", output_file_name)
to_latex(df_heu_c_h_n, df_cplex_c_h_n, "Cloud distribution, hard real time constraints without essential gateways",   "tab:cloud_hard_n_esc", output_file_name)