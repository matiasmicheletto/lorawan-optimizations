import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV data
data = pd.read_csv('summary.csv')  # Replace with your CSV file path

# Create a bar plot
plt.figure(figsize=(10, 6))
colors = {
    'Greedy': 'blue',
    'GA': 'green',
    'GA - Warm start': 'red'
}   

# Get unique instances and solvers
instances = data['Instance Name']
solvers = data['Solver'].unique()

# Set bar width and positions
bar_width = 0.2
index = np.arange(len(instances))

# Plotting
for i, solver in enumerate(solvers):
    subset = data[data['Solver'] == solver]
    # Align the bars with their correct positions based on the index of instances
    for j in range(len(subset)):
        plt.bar(index[j] + i * bar_width, subset['Cost'].values[j], bar_width,
                color=colors.get(solver, 'gray'), label=solver if j == 0 else "")

plt.xlabel('Instance Name')
plt.ylabel('Cost')
plt.title('Cost Values by Instance and Solver')
plt.xticks(index + (len(solvers) - 1) * bar_width / 2, instances, rotation=45)
plt.legend(title='Solver')
plt.tight_layout()
plt.show()
