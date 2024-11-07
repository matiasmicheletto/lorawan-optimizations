import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 12


cplex = [ # GW, E, UF
    [15,595,0.006514],
    [10,687,0.01002],
    [8,767,0.01101],
    [6,904,0.01205],
    [5,1016,0.01906],
    [4,1164,0.02407],
    [3,1386,0.03812],
    [2,1708,0.08256],
    [1,2194,0.31052]
] 

greedy = [
    [12,752,0.012529],
    [11,782,0.013530],
    [10,789,0.014530],
    [9,791,0.015527],
    [7,854,0.016062],
    [6,904,0.019544],
    [5,1016,0.027116],
    [4,1166,0.045172]
]

moga = [
    [30,1292,0.012],
    [29,1367,0.012],
    [26,1427,0.014],
    [16,2294,0.036],
    [15,2504,0.05]
]

"""
Figuras a y b
"""

def e_vs_g(axs, col):    
    # Plot E vs GW for cplex
    for i in range(len(cplex) - 1):
        axs.plot([cplex[i][col], cplex[i+1][col]], [cplex[i][0], cplex[i+1][0]], color='red')
        axs.scatter(cplex[i][col], cplex[i][0], color='red')
    axs.scatter(cplex[-1][col], cplex[-1][0], color='red')
    axs.annotate('CPLEX', (cplex[0][col], cplex[0][0]), textcoords="offset points", xytext=(5,5), ha='center', color='red')

    # Plot E vs GW for greedy
    for i in range(len(greedy) - 1):
        axs.plot([greedy[i][col], greedy[i+1][col]], [greedy[i][0], greedy[i+1][0]], color='blue')
        axs.scatter(greedy[i][col], greedy[i][0], color='blue')
    axs.scatter(greedy[-1][col], greedy[-1][0], color='blue')
    axs.annotate('Greedy', (greedy[0][col], greedy[0][0]), textcoords="offset points", xytext=(5,5), ha='center', color='blue')

    # Plot E vs GW for moga
    for i in range(len(moga) - 1):
        axs.plot([moga[i][col], moga[i+1][col]], [moga[i][0], moga[i+1][0]], color='black')
        axs.scatter(moga[i][col], moga[i][0], color='black')
    axs.scatter(moga[-1][col], moga[-1][0], color='black')
    axs.annotate('MOGA', (moga[0][col], moga[0][0]), textcoords="offset points", xytext=(5,5), ha='center', color='black')


fig, axs = plt.subplots(1, 1, figsize=(6, 6))
e_vs_g(axs,1)
axs.set_xlabel("Energy")
axs.set_ylabel("Gateway")
plt.grid(True)
plt.tight_layout()
plt.show()

fig, axs2 = plt.subplots(1, 1, figsize=(6, 6))
e_vs_g(axs2,2)
axs2.set_xlabel("Utilization")
axs2.set_ylabel("Gateway")
plt.grid(True)
plt.tight_layout()
plt.show()

"""
Figura c
"""

E1 = [2194,2256,2344]
U1 = [0.3500,0.3200,0.3105]

E2 = [1708,1708,1724]
U2 = [0.200000,0.100000,0.090000]

E3 = [1386,1396,1420,1420,1978]
U3 = [0.060000,0.050000,0.040000,0.039000,0.038120]

E4 = [1164,1170,1226,1242,1242]
U4 = [0.050000,0.040000,0.030000,0.027000,0.024500]

E5 = [1016,1016,1032,1062,1064,1104,1144]
U5 = [0.035000,0.030000,0.025000,0.023000,0.020000,0.018000,0.017000]

E6 = [904,912,938,946,962,976,994]
U6 = [0.020000,0.018000,0.016000,0.015000,0.014000,0.013000,0.012500]

E8 = [878,814,786,770,767,767]
U8 = [9.0000e-03,1.0000e-02,1.1010e-02,1.5000e-02,2.0000e-02,3.0000e-02]

E10 = [687,687,695,734,740,972]
U10 = [3.0000e-02,2.0000e-02,1.0000e-02,8.0000e-03,7.0000e-03,6.0000e-03]

E15 = [585,585,585,585,593,593,595]
U15 = [2.0000e-02,1.0000e-02,9.0000e-03,8.0000e-03,7.0000e-03,6.6000e-03,6.5140e-03]


def plotdata(axs, X, Y, color, label):
    axs.plot(X, Y, color=color, label=label)
    axs.scatter(X, Y, color=color)
    axs.annotate(label, (X[-1], Y[-1]), textcoords="offset points", xytext=(5,5), ha='center', color=color)


fig, axs = plt.subplots(1, 1, figsize=(6, 6))    
plotdata(axs, U1, E1, 'red', 'G=1')
plotdata(axs, U2, E2, 'blue', 'G=2')
plotdata(axs, U3, E3, 'brown', 'G=3')
plotdata(axs, U4, E4, 'black', 'G=4')
plotdata(axs, U5, E5, 'red', 'G=5')
plotdata(axs, U6, E6, 'blue', 'G=6')
plotdata(axs, U8, E8, 'green', 'G=8')
plotdata(axs, U10, E10, 'brown', 'G=10')
plotdata(axs, U15, E15, 'black', 'G=15')
axs.set_xlabel("Utilization")
axs.set_ylabel("Energy")
plt.grid(True)
plt.tight_layout()
plt.show()


"""
mogaWME = [
    [1108,17]
    [1122,14]
    [1128,5]
    [1210,4]
]

mogaWMU = [
    [0.0130573,26
    0.0160623,25
    0.0311135,19
    0.0311396,18
    0.0321245,8
    0.0391396,7
    0.0501787,5]
]
"""

