import pandas as pd
import argparse
import matplotlib.pyplot as plt
import matplotlib as mpl

# This program takes a CSV file as input and plots 3D and 2D graphs.
# Usage:
# python data-plotter-3d-2d.py data.csv

# Adjust font for latex rendering
mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 12

# Parse command line arguments to get filename
parser = argparse.ArgumentParser(description='Plot 3D and 2D graphs from CSV file')
parser.add_argument('filename', type=str, help='CSV file to plot')
args = parser.parse_args()
file_path = args.filename
print(f"Plotting data from {file_path}...")

# Read the CSV file
data = pd.read_csv(file_path)

if data.shape[1] < 5:
    raise ValueError("CSV file must have at least 3 columns")

# Extract columns for plotting
opt_name = data.columns[0] # Optimizer names: GreedyMO, MOGA2, MOGA2WS
col1 = data.columns[3] # GW
col2 = data.columns[4] # E
col3 = data.columns[5] # UF

# Data colors for each optimizer
colors = {'GreedyMO': 'blue', 'MOGA2': 'red', 'MOGA2WS': 'green'}

# Function to check if a dominates b
def dominates(a, b):
    return all(x >= y for x, y in zip(a, b)) and any(x > y for x, y in zip(a, b))

def getPareto(vector):
    pareto_front = []
    for i, a in enumerate(vector):
        for j, b in enumerate(vector):
            if i != j and dominates(a, b):
                break
        else:
            pareto_front.append(a)
    # Sort the Pareto front by the first dimension for ordered plotting
    pareto_front.sort(key=lambda x: x[0])
    return pareto_front

def plotdata(axs, data, pareto, col1, col2, title, xlabel, ylabel):
    for i in range(len(pareto) - 1):
        axs.plot([pareto[i][0], pareto[i+1][0]], [pareto[i][1], pareto[i+1][1]], color='red')
        axs.scatter(pareto[i][0], pareto[i][1], color='red')
    axs.scatter(pareto[-1][0], pareto[-1][1], color='red')

        

    #for opt in colors:
        #opt_data = data[data[opt_name] == opt]
        #axs.scatter(opt_data[col1], opt_data[col2], c=colors[opt], alpha=0.7, label=opt)
    axs.set_title(title)
    axs.set_xlabel(xlabel)
    axs.set_ylabel(ylabel)
    #axs.legend()
    axs.grid(True)


# Discard dominated solutions
pareto_GW_E = getPareto(data[[col1, col2]].values)
pareto_E_GW = getPareto(data[[col2, col1]].values)
pareto_E_UF = getPareto(data[[col2, col3]].values)
pareto_GW_UF = getPareto(data[[col1, col3]].values)
pareto_UF_GW = getPareto(data[[col3, col1]].values)

"""
# Plot the data
fig, axs = plt.subplots(1, 3, figsize=(15, 5)) # Create the figure and axes for the three plots

#plotdata(axs[0], data, pareto_GW_E, col1, col2, 'GW vs E', 'GW', 'E')
plotdata(axs[0], data, pareto_E_GW, col1, col2, '', 'E', 'GW')
plotdata(axs[1], data, pareto_E_UF, col2, col3, 'E vs UF', 'E', 'UF')
#plotdata(axs[2], data, pareto_GW_UF, col1, col3, 'GW vs UF', 'GW', 'UF')
plotdata(axs[2], data, pareto_UF_GW, col3, col1, '', 'UF', 'GW')


# Adjust layout and save figure
plt.tight_layout()
#plt.show()
plt.savefig(file_path.replace('.csv', '.png'))
"""

# Print the Pareto front in pair of values:
print("Pareto front for E vs GW:")
for p in pareto_E_GW:
    print(p)

#print("Pareto front for E vs UF:")
#for p in pareto_E_UF:
#    print(p)

print("Pareto front for UF vs GW:")
for p in pareto_UF_GW:
    print(p)


print("Done!")
