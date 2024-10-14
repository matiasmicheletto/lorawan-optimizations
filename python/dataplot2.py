
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 24

data = [ # Gateway, Energy, Utilization Factor
    [69,1070,0.00601353],
    [61,1082,0.0110371],
    [65,1003,0.00702913],
    [57,1048,0.0140583],
    [67,1059,0.0130573],
    [57,986,0.0100503],
    [71,1141,0.00702913]
]

Gateways = [x[0] for x in data]
Energy = [x[1] for x in data]
Utilization_factor = [x[2] for x in data]

plt.figure(figsize=(8, 6))
plt.scatter(Energy, Gateways, marker='o', color='b', s=100)
plt.title('Pareto Front')
plt.xlabel('Energy')
plt.ylabel('Gateways')

plt.grid(True)
plt.tight_layout()
plt.savefig('pareto_e_gw.png', bbox_inches='tight')
plt.show()


plt.figure(figsize=(8, 6))
plt.scatter(Utilization_factor, Gateways, marker='o', color='b', s=100)
plt.title('Pareto Front')
plt.xlabel('Utilization Factor')
plt.ylabel('Gateways')
plt.grid(True)
plt.tight_layout()
plt.savefig('pareto_uf_gw.png')
plt.show()


plt.figure(figsize=(8, 6))
plt.scatter(Utilization_factor, Energy, marker='o', color='b', s=100)
plt.title('Pareto Front')
plt.xlabel('Utilization Factor')
plt.ylabel('Energy')
plt.grid(True)
plt.tight_layout()
plt.savefig('pareto_uf_e.png')
plt.show()
