def to_latex(df, caption, label, filename):
    header = ('\\begin{table}[htb]\n'  
        '\t\\centering\n'  
        '\t\\caption{'+caption+'} \n'  
        '\t\\label{'+label+'} \n'  
        '\t\\begin{tabular}{ |c|c|c|c|c| } \n'  
        '\t\t\\hline\n'  
        '\t\t\\multirow{2}{*}{Edad} & \\multicolumn{2}{c|}{No fumador} & \\multicolumn{2}{c|}{Fumador} \\\\ \n'  
        '\t\t\\cline{2-5} \n'  
        '& Masculino & Femenino & Masculino & Femenino \\\\ \n'  
        '\t\t\\hline\n')  
    footer = ('\t\t\\hline \n'  
        '\t\\end{tabular} \n'  
        '\\end{table} \n')  
    content = ''  
    for row in df.index:  
        columndata = " & ".join([str(round(df.loc[row, col],2)) for col in df.columns])  
        content = content + "\t\t" + row + " & " + columndata + " \\\\ \n"  
    with open(filename, 'a') as latexfile:  
        latexfile.write(header+content+footer)