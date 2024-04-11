import pandas as pd
import to_latex_table
  

def average_group(data, method, dist, rt): # Filter, group and average
    filtered_df = data[(data['Dist'] == dist) & (data['RT'] == rt) & (data['Method'] == method)]
    grouped_df = filtered_df.groupby(['Map', 'ED'])[['FO', 'G', 'E', 'U', 'T']]
    averaged_df = grouped_df.mean().reset_index()
    return averaged_df


def to_latex(dfl, dfr, caption, label, filename): # Convert to LaTeX table
    header = ('\\begin{table}[htb]\n'  
        '\t\\centering\n'  
        '\t\\caption{'+caption+'} \n'  
        '\t\\label{'+label+'} \n'  
        '\t\\begin{tabular}{ |c|c|c|c|c| } \n'  
        '\t\t\\hline\n'  
        '\t\t\\multirow{2}{*}{Edad} & \\multicolumn{2}{c|}{Greedy heu.} & \\multicolumn{2}{c|}{CPLEX} \\\\ \n'  
        '\t\t\\cline{2-5} \n'  
        '& Masculino & Femenino & Masculino & Femenino \\\\ \n'  
        '\t\t\\hline\n')  
    footer = ('\t\t\\hline \n'  
        '\t\\end{tabular} \n'  
        '\\end{table} \n')  
    content = ''  
    for row in dfl.index:  
        row_str = str(row)
        columndata = " & ".join([str(round(dfl.loc[row, col],2)) for col in dfl.columns])  
        content = content + "\t\t" + row_str + " & " + columndata + " \\\\ \n"  
    #with open(filename, 'a') as latexfile:  
    #    latexfile.write(header+content+footer)
    print(content)


# Load dataframe from file
data = pd.read_csv('../dat/comparative_results.csv')  
# Rename columns  
data.rename(columns = {  
    'Metodo': 'Method',  
    'Mapa': 'Map'
    }, inplace = True)  
# Rename categorical data
data.replace({  
    'Method': {  
        'HEURISTICA': 'HEU'
    },
    'Dist': {
        'UNIFORME': 'UNIFORM'
    }
}, inplace = True)
# Zero imputation
data['TO'] = data['TO'].fillna(0)


# Build each table
df_heu = average_group(data, 'HEU', 'UNIFORM', 'SOFT')
df_cplex = average_group(data, 'CPLEX', 'UNIFORM', 'SOFT')
to_latex(df_heu, df_cplex, "Uniform distribution, soft real time restrictions", "tab:unif_soft", "tablas.tex")
