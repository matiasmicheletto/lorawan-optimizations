import pandas as pd

def init_dataframe(input_file):
    data = pd.read_csv(input_file)  
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
    data['Escenc'] = data['Escenc'].fillna('NO')
    return data

def average_group(data, method, dist, rt, esc = None): # Filter, group and average
    filtered_df = data[(data['Dist'] == dist) & (data['RT'] == rt) & (data['Method'] == method)]
    if(esc is not None):
        filtered_df = filtered_df[filtered_df['Escenc'] == esc]
    grouped_df = filtered_df.groupby(['Map', 'ED'])[['FO', 'G', 'E', 'U', 'T']]
    averaged_df = grouped_df.mean().reset_index()
    return averaged_df

def to_latex(dfl, dfr, caption, label, filename): # Convert to LaTeX table
    header = ('\\begin{table}[htb]\n'  
        '\t\\centering\n'  
        '\t\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|c|c|}\n'  
        '\t\t\\hline\n'  
        '\t\t\\multirow{2}{*}{ED} & \\multirow{2}{*}{Map} & \\multicolumn{5}{c|}{Greedy Heuristic} & \\multicolumn{5}{c|}{CPLEX}\\\\ \n'  
        '\t\t\\cline{3-12}\n'  
        '&& OF & G & E & U & Time & OF & G & E & U & Time\\\\ \n'
        '\t\t\\hline\n')  
    footer = ('\t\t\\hline \n'  
        '\t\\end{tabular} \n'  
        '\t\\caption{'+caption+'} \n'  
        '\t\\label{'+label+'} \n'  
        '\\end{table} \n\n')  
    content = ''  
    for row in dfl.index:  
        columndata_l = " & ".join([str(round(dfl.loc[row, col],3)) for col in dfl.columns]) 
        columndata_r = " & ".join([str(round(dfr.loc[row, col],3)) for col in dfr.columns[2:]]) if row < len(dfr) else "& & & & "
        content = content + "\t\t" + columndata_l + " & " + columndata_r + " \\\\ \n"  
    with open(filename, 'a') as latexfile:  
        latexfile.write(header+content+footer)
