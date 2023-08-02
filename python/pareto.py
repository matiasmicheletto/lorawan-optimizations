import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv("comparativa_ilp_greedy.csv")
#df.sort_values(by="eng", inplace=True)

solver_colors = {}
solvers = df["solver"].unique()
color_map = plt.cm.get_cmap('tab10', len(solvers))
for i, solver in enumerate(solvers):
    solver_colors[solver] = color_map(i)

# Create the scatter plot for each solver
for solver in solver_colors:
    solver_data = df[df["solver"] == solver]
    #plt.scatter(solver_data["gw"], solver_data["eng"], label=solver, marker='o', color=solver_colors[solver])
    plt.scatter(solver_data["gw"], solver_data["tiempo"], label=solver, marker='o', color=solver_colors[solver])

plt.xlabel("GW Used")
plt.ylabel("UF")
plt.title("UF vs. GW")
plt.legend()
plt.grid(True)

plt.show()
