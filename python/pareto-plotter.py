import pandas as pd
import argparse
import matplotlib.pyplot as plt

# This program takes a CSV file as input and plots 3D and 2D graphs.
# Usage:
# python data-plotter-3d-2d.py data.csv

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
col1 = data.columns[2] # GW
col2 = data.columns[3] # E
col3 = data.columns[4] # UF

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

def plotdata(axs, data, pareto, col1, col2, title):
    axs.scatter(data[col1], data[col2], color='blue', alpha=0.7)
    axs.scatter([v[0] for v in pareto], [v[1] for v in pareto], color='red', alpha=0.7)
    for i in range(len(pareto) - 1):
        axs.plot([pareto[i][0], pareto[i+1][0]], [pareto[i][1], pareto[i+1][1]], color='red')
    axs.set_xlabel(col1)
    axs.set_ylabel(col2)
    axs.set_title(title)
    axs.grid


# Create the figure and axes for the three plots
fig, axs = plt.subplots(1, 3, figsize=(15, 5))

# Discard dominated solutions
pareto_GW_E = getPareto(data[[col1, col2]].values)
pareto_E_UF = getPareto(data[[col2, col3]].values)
pareto_GW_UF = getPareto(data[[col1, col3]].values)

# Plot the data
plotdata(axs[0], data, pareto_GW_E, col1, col2, 'GW vs E')
plotdata(axs[1], data, pareto_E_UF, col2, col3, 'E vs UF')
plotdata(axs[2], data, pareto_GW_UF, col1, col3, 'GW vs UF')

# Adjust layout and save figure
plt.tight_layout()
#plt.show()
plt.savefig(file_path.replace('.csv', '.png'))

print("Done!")
