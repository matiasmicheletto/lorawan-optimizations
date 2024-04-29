from utils import *


input_file_name = '../results/all.csv'
output_file_name = 'tables.tex'

# Write first line
with open(output_file_name, 'w') as latexfile:  
    latexfile.write('\\section\{Results\} \\label\{appendix:tables\}\n\n')

# Load dataframe from file and prepare columns
data = init_dataframe(input_file_name)

# Build each table averaging values
heu_unif_hard_n_esc_100 =   average_group(data, 'HEU', 'UNIFORM', 'HARD', 'NO', 100)
cplex_unif_hard_n_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'NO', 100 )
heu_unif_medium_n_esc_100 = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'NO', 100)
cplex_unif_medium_n_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'NO', 100)
heu_unif_soft_n_esc_100 =  average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'NO', 100)
cplex_unif_soft_n_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'NO', 100)

heu_cloud_hard_n_esc_100 = average_group(data, 'HEU', 'CLOUD', 'HARD', 'NO', 100) 
cplex_cloud_hard_n_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'NO', 100) 
heu_cloud_medium_n_esc_100 = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'NO', 100) 
cplex_cloud_medium_n_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'NO', 100)
heu_cloud_soft_n_esc_100 = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'NO', 100) 
cplex_cloud_soft_n_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'NO', 100)

heu_unif_hard_esc_100 = average_group(data, 'HEU', 'UNIFORM', 'HARD', 'SI', 100)
cplex_unif_hard_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'SI', 100)
heu_unif_medium_esc_100 = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'SI', 100)
cplex_unif_medium_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'SI', 100)
heu_unif_soft_esc_100 = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'SI', 100)
cplex_unif_soft_esc_100 = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'SI', 100)

heu_cloud_hard_esc_100 = average_group(data, 'HEU', 'CLOUD', 'HARD', 'SI', 100)
cplex_cloud_hard_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'SI', 100)
heu_cloud_medium_esc_100 = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'SI', 100)
cplex_cloud_medium_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'SI', 100)
heu_cloud_soft_esc_100 = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'SI', 100)
cplex_cloud_soft_esc_100 = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'SI', 100)

heu_unif_hard_n_esc_500 = average_group(data, 'HEU', 'UNIFORM', 'HARD', 'NO', 500)
cplex_unif_hard_n_esc_500 = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'NO', 500)

heu_unif_hard_n_esc_1000 = average_group(data, 'HEU', 'UNIFORM', 'HARD', 'NO', 1000)
cplex_unif_hard_n_esc_1000 = average_group(data, 'CPLEX', 'UNIFORM', 'HARD', 'NO', 1000)

heu_unif_medium_n_esc_500 = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'NO', 500)
cplex_unif_medium_n_esc_500 = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'NO', 500)

heu_unif_medium_n_esc_1000 = average_group(data, 'HEU', 'UNIFORM', 'MEDIUM', 'NO', 1000)
cplex_unif_medium_n_esc_1000 = average_group(data, 'CPLEX', 'UNIFORM', 'MEDIUM', 'NO', 1000)

heu_unif_soft_n_esc_500 = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'NO', 500)
cplex_unif_soft_n_esc_500 = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'NO', 500)

heu_unif_soft_n_esc_1000 = average_group(data, 'HEU', 'UNIFORM', 'SOFT', 'NO', 1000)
cplex_unif_soft_n_esc_1000 = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT', 'NO', 1000)

heu_cloud_hard_n_esc_500 = average_group(data, 'HEU', 'CLOUD', 'HARD', 'NO', 500)
cplex_cloud_hard_n_esc_500 = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'NO', 500)

heu_cloud_hard_n_esc_1000 = average_group(data, 'HEU', 'CLOUD', 'HARD', 'NO', 1000)
cplex_cloud_hard_n_esc_1000 = average_group(data, 'CPLEX', 'CLOUD', 'HARD', 'NO', 1000)

heu_cloud_medium_n_esc_500 = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'NO', 500)
cplex_cloud_medium_n_esc_500 = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'NO', 500)

heu_cloud_medium_n_esc_1000 = average_group(data, 'HEU', 'CLOUD', 'MEDIUM', 'NO', 1000)
cplex_cloud_medium_n_esc_1000 = average_group(data, 'CPLEX', 'CLOUD', 'MEDIUM', 'NO', 1000)

heu_cloud_soft_n_esc_500 = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'NO', 500)
cplex_cloud_soft_n_esc_500 = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'NO', 500)

heu_cloud_soft_n_esc_1000 = average_group(data, 'HEU', 'CLOUD', 'SOFT', 'NO', 1000)
cplex_cloud_soft_n_esc_1000 = average_group(data, 'CPLEX', 'CLOUD', 'SOFT', 'NO', 1000)


# Write latex tables to output file
to_latex(heu_unif_hard_n_esc_100, cplex_unif_hard_n_esc_100, "Uniform distribution, 100x100 map, hard real time constraints without essential gateways","tab:unif_hard_n_esc_100", output_file_name)
to_latex(heu_unif_medium_n_esc_100, cplex_unif_medium_n_esc_100, "Uniform distribution, 100x100 map, medium real time constraints without essential gateways","tab:unif_medium_n_esc_100", output_file_name)
to_latex(heu_unif_soft_n_esc_100, cplex_unif_soft_n_esc_100, "Uniform distribution, 100x100 map, soft real time constraints without essential gateways","tab:unif_soft_n_esc_100", output_file_name)

to_latex(heu_cloud_hard_n_esc_100, cplex_cloud_hard_n_esc_100, "Cloud distribution, 100x100 map, hard real time constraints without essential gateways","tab:cloud_hard_n_esc_100", output_file_name)
to_latex(heu_cloud_medium_n_esc_100, cplex_cloud_medium_n_esc_100, "Cloud distribution, 100x100 map, medium real time constraints without essential gateways","tab:cloud_medium_n_esc_100", output_file_name)
to_latex(heu_cloud_soft_n_esc_100, cplex_cloud_soft_n_esc_100, "Cloud distribution, 100x100 map, soft real time constraints without essential gateways","tab:cloud_soft_n_esc_100", output_file_name)


to_latex(heu_unif_hard_esc_100, cplex_unif_hard_esc_100, "Uniform distribution, 100x100 map, hard real time constraints with essential gateways","tab:unif_hard_esc_100", output_file_name)
to_latex(heu_unif_medium_esc_100, cplex_unif_medium_esc_100, "Uniform distribution, 100x100 map, medium real time constraints with essential gateways","tab:unif_medium_esc_100", output_file_name)
to_latex(heu_unif_soft_esc_100, cplex_unif_soft_esc_100, "Uniform distribution, 100x100 map, soft real time constraints with essential gateways","tab:unif_soft_esc_100", output_file_name)

to_latex(heu_cloud_hard_esc_100, cplex_cloud_hard_esc_100, "Cloud distribution, 100x100 map, hard real time constraints with essential gateways","tab:cloud_hard_esc_100", output_file_name)
to_latex(heu_cloud_medium_esc_100, cplex_cloud_medium_esc_100, "Cloud distribution, 100x100 map, medium real time constraints with essential gateways","tab:cloud_medium_esc_100", output_file_name)
to_latex(heu_cloud_soft_esc_100, cplex_cloud_soft_esc_100, "Cloud distribution, 100x100 map, soft real time constraints with essential gateways","tab:cloud_soft_esc_100", output_file_name)

to_latex(heu_unif_hard_n_esc_500, cplex_unif_hard_n_esc_500, "Uniform distribution, 500x500 map, hard real time constraints","tab:unif_hard_n_esc_500", output_file_name)
to_latex(heu_unif_hard_n_esc_1000, cplex_unif_hard_n_esc_1000, "Uniform distribution, 1000x1000 map, hard real time constraints","tab:unif_hard_n_esc_1000", output_file_name)

to_latex(heu_unif_medium_n_esc_500, cplex_unif_medium_n_esc_500, "Uniform distribution, 500x500 map, medium real time constraints","tab:unif_medium_n_esc_500", output_file_name)
to_latex(heu_unif_medium_n_esc_1000, cplex_unif_medium_n_esc_1000, "Uniform distribution, 1000x1000 map, medium real time constraints","tab:unif_medium_n_esc_1000", output_file_name)

to_latex(heu_unif_soft_n_esc_500, cplex_unif_soft_n_esc_500, "Uniform distribution, 500x500 map, soft real time constraints","tab:unif_soft_n_esc_500", output_file_name)
to_latex(heu_unif_soft_n_esc_1000, cplex_unif_soft_n_esc_1000, "Uniform distribution, 1000x1000 map, soft real time constraints","tab:unif_soft_n_esc_1000", output_file_name)

to_latex(heu_cloud_hard_n_esc_500, cplex_cloud_hard_n_esc_500, "Cloud distribution, 500x500 map, hard real time constraints without essential gateways","tab:cloud_hard_n_esc_500", output_file_name)
to_latex(heu_cloud_hard_n_esc_1000, cplex_cloud_hard_n_esc_1000, "Cloud distribution, 1000x1000 map, hard real time constraints without essential gateways","tab:cloud_hard_n_esc_1000", output_file_name)

to_latex(heu_cloud_medium_n_esc_500, cplex_cloud_medium_n_esc_500, "Cloud distribution, 500x500 map, medium real time constraints without essential gateways","tab:cloud_medium_n_esc_500", output_file_name)
to_latex(heu_cloud_medium_n_esc_1000, cplex_cloud_medium_n_esc_1000, "Cloud distribution, 1000x1000 map, medium real time constraints without essential gateways","tab:cloud_medium_n_esc_1000", output_file_name)

to_latex(heu_cloud_soft_n_esc_500, cplex_cloud_soft_n_esc_500, "Cloud distribution, 500x500 map, soft real time constraints without essential gateways","tab:cloud_soft_n_esc_500", output_file_name)
to_latex(heu_cloud_soft_n_esc_1000, cplex_cloud_soft_n_esc_1000, "Cloud distribution, 1000x1000 map, soft real time constraints without essential gateways","tab:cloud_soft_n_esc_1000", output_file_name)
