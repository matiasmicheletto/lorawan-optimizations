# Energy minimization
# Energy=[2373,1670,1285,984,834,724,646,606,581,565,553,546,544] # x axis
# Gateways=[1,2,3,5,7,9,11,13,15,17,19,21,22] # y axis


# GW minimization
# Utilization_factor=[0.1, 0.05,0.03,0.02, 0.01,0.009,0.008,0.007,0.006,0.005,0.00406] # x axis
# Gateways=[2,3,4,5,7,8,9,9,10,12,13] # y axis

# UF minimization
# Utilization_factor=[0.0055,0.00551,0.00602,0.00651,0.00701,0.0105] # x axis
# Energy=[1055,983,895,846,748,700] # y axis


import matplotlib.pyplot as plt
import matplotlib as mpl


mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 24

# Energy minimization
Energy = [2373, 1670, 1285, 984, 834, 724, 646, 606, 581, 565, 553, 546, 544] # x axis
Gateways = [1, 2, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 22] # y axis

plt.figure(figsize=(8, 6))
plt.plot(Energy, Gateways, marker='o', linestyle='-', color='b', linewidth=2)
#plt.title('Energy Minimization')
plt.xlabel('Energy')
plt.ylabel('Gateways')

plt.grid(True)
plt.tight_layout()
plt.savefig('energy_minimization.png', bbox_inches='tight')
plt.show()

# GW minimization
Utilization_factor = [0.1, 0.05, 0.03, 0.02, 0.01, 0.009, 0.008, 0.007, 0.006, 0.005, 0.00406] # x axis
Gateways = [2, 3, 4, 5, 7, 8, 9, 9, 10, 12, 13] # y axis

plt.figure(figsize=(8, 6))
plt.plot(Utilization_factor, Gateways, marker='o', linestyle='-', color='r', linewidth=2)
#plt.title('GW Minimization')
plt.xlabel('Utilization Factor')
plt.ylabel('Gateways')
plt.grid(True)
plt.tight_layout()
plt.savefig('gw_minimization.png')
plt.show()

# UF minimization
Utilization_factor = [0.0055, 0.00551, 0.00602, 0.00651, 0.00701, 0.0105] # x axis
Energy = [1055, 983, 895, 846, 748, 700] # y axis

plt.figure(figsize=(8, 6))
plt.plot(Utilization_factor, Energy, marker='o', linestyle='-', color='g', linewidth=2)
#plt.title('UF Minimization')
plt.xlabel('Utilization Factor')
plt.ylabel('Energy')
plt.grid(True)
plt.tight_layout()
plt.savefig('uf_minimization.png')
plt.show()
