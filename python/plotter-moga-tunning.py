import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 16


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

def plotdata(ax, pareto, xlabel, ylabel, color, label, legend=True):
    x_vals, y_vals = zip(*pareto)  # Extract points
    ax.plot(x_vals, y_vals, color=color, linestyle='-', marker='o', label=label)  # Plot line with markers
    ax.scatter(x_vals, y_vals, color=color)  # Ensure points are highlighted       
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.grid(True)
    if legend:
        ax.legend() 
    else: 
        #ax.text(x_vals[0]*1.1, y_vals[0], label, fontsize=12, color=color, verticalalignment='bottom', horizontalalignment='right')
        ax.annotate(
            label,
            (x_vals[0], y_vals[0]),  # Data coordinates
            textcoords="offset points",  # Move relative to the data point
            xytext=(10, 0),  # Move 10 points to the right
            fontsize=12,
            color=color,
            verticalalignment='bottom',
            horizontalalignment='left'  # Adjusted to match the new position
        )

file_path = "MOGA_tunning_server.csv"

data = pd.read_csv(file_path)



# Generic crossover data
data_generic_xo = data[data['crossover type'] == 'generic_cross']
pareto_generic_E_GW = getPareto(data_generic_xo[['e', 'gw']].values)
pareto_generic_UF_GW = getPareto(data_generic_xo[['uf', 'gw']].values)


# print data points csv format
print(pareto_generic_E_GW)
print(pareto_generic_UF_GW)

for i in pareto_generic_E_GW:
    print(f'{i[0]},{i[1]}')
print()
for i in pareto_generic_UF_GW:
    print(f'{i[0]},{i[1]}')


# Custom crossover data
data_custom_xo = data[data['crossover type'] == 'custom_cross']
pareto_custom_E_GW = getPareto(data_custom_xo[['e', 'gw']].values)
pareto_custom_UF_GW = getPareto(data_custom_xo[['uf', 'gw']].values)



# Plots

fig, ax = plt.subplots(figsize=(10, 6))
plotdata(ax, pareto_generic_UF_GW, 'Utilization', 'Gateway', 'red', 'Original', False)
plotdata(ax, pareto_custom_UF_GW, 'Utilization', 'Gateway', 'blue', 'Variant', False)
plt.tight_layout()
#plt.show()
plt.savefig("MOGA_original_vs_variant_energy.png")

#fig, axs = plt.subplots(1, 2, figsize=(15, 5))

#plotdata(axs[0], pareto_generic_E_GW, 'Energy [W-slot]', 'Gateway', 'red', 'Original', False)
#plotdata(axs[1], pareto_generic_UF_GW, 'Utilization', 'Gateway', 'red', 'Original', False)

#plotdata(axs[0], pareto_custom_E_GW, 'Energy [W-slot]', 'Gateway', 'blue', 'Variant', False)
#plotdata(axs[1], pareto_custom_UF_GW, 'Utilization', 'Gateway', 'blue', 'Variant', False)

#plt.tight_layout()
#plt.show()
#plt.savefig(file_path.replace('.csv', '.png'))
