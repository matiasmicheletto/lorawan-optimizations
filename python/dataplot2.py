
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['mathtext.fontset'] = 'stix'  
mpl.rcParams['font.family'] = 'STIXGeneral'  
mpl.rcParams['font.size'] = 24


plainData = """23,575,0.0353282
25,490,0.0302523
25,504,0.0252271
22,549,0.0377711
24,553,0.0252271
25,522,0.0454292
24,553,0.0252271
24,553,0.0252271
25,522,0.0454292
25,522,0.0454292
24,553,0.0252271
24,553,0.0252271
25,522,0.0454292
24,553,0.0252271
24,553,0.0252271
25,504,0.0252271
25,504,0.0252271
25,522,0.0454292
25,490,0.0302523
23,575,0.0353282
24,553,0.0252271
22,549,0.0377711
25,504,0.0252271"""


# parse plain data
data = []
for line in plainData.split('\n'):
    data.append([float(x) for x in line.split(',')])
    

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
