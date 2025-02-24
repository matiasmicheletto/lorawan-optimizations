import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Delaunay
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


# GW, E, UF
cplex = np.array([ 
    #[15,595,0.006514],
    [10,687,0.01002],
    [8,767,0.01101],
    [6,904,0.01205],
    [5,1016,0.01906],
    [4,1164,0.02407],
    #[3,1386,0.03812],
    #[2,1708,0.08256],
    #[1,2194,0.31052]
]) 

greedy = np.array([
    [12,752,0.012529],
    [11,782,0.013530],
    [10,789,0.014530],
    [9,791,0.015527],
    [7,854,0.016062],
    [6,904,0.019544],
    [5,1016,0.027116],
    [4,1166,0.045172]
])

moga = np.array([
    [30,1292,0.012],
    [29,1367,0.012],
    [26,1427,0.014],
    [16,2294,0.036],
    [15,2504,0.05]
])

mogaWSE = np.array([
    [1108,17],
    [1122,14],
    [1128,5],
    [1210,4]
])

mogaWSU = np.array([
    [0.0130573,26],
    [0.0160623,25],
    [0.0311135,19],
    [0.0311396,18],
    [0.0321245,8],
    [0.0391396,7],
    [0.0501787,5]
])


def compute_volume(x, y, z):
    points = np.column_stack((x, y))  # Only (x, y) for triangulation
    tri = Delaunay(points)  # Create a triangulated mesh
    volume = 0.0

    for simplex in tri.simplices:
        x1, y1, z1 = x[simplex[0]], y[simplex[0]], z[simplex[0]]
        x2, y2, z2 = x[simplex[1]], y[simplex[1]], z[simplex[1]]
        x3, y3, z3 = x[simplex[2]], y[simplex[2]], z[simplex[2]]

        # Compute the signed volume of the tetrahedron with the base at (x, y, 0)
        matrix = np.array([
            [x1, y1, z1],
            [x2, y2, z2],
            [x3, y3, z3]
        ])
        volume += np.linalg.det(matrix) / 6.0  # Determinant method

    return abs(volume)  # Volume is always positive

def plot_surface(fig, ax, x, y, z, labels, legend):
    points = np.column_stack((x, y))  # Only (x, y) for triangulation
    tri = Delaunay(points)  # Create a triangulated mesh

    # Plot the surface
    ax.plot_trisurf(x, y, z, triangles=tri.simplices, edgecolor='k', alpha=0.8)

    # Plot the raw data points
    ax.scatter(x, y, z, color='red', s=30)

    # Project surface onto (x, y, 0) plane
    for simplex in tri.simplices:
        triangle = [(x[i], y[i], 0) for i in simplex]
        ax.add_collection3d(Poly3DCollection([triangle], color='gray', alpha=0.3))

    # Labels and view angle
    ax.set_xlabel(labels[0])
    ax.set_ylabel(labels[1])
    ax.set_zlabel(labels[2])

    # Insert legend
    ax.text(x[-1], y[-1], z[-1], legend, color='black', fontsize=12, ha='center', va='center')

    ax.view_init(elev=30, azim=-60)  # Adjust view angle


fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')

for data, name in zip([cplex, greedy, moga], ["CPLEX", "Greedy", "MOGA"]):
    gw = data[:,0]
    e = data[:,1]
    uf = data[:,2]

    vol = compute_volume(e, uf, gw)
    print(f"Computed volume for {name}: {vol:.4f}")

    plot_surface(fig, ax, gw, e, uf, labels=["E", "UF", "GW"], legend=name)


plt.show()



"""
gw = cplex[:,0]
e = cplex[:,1]
uf = cplex[:,2]
vol = compute_volume(e, uf, gw)
print(f"Computed volume: {vol:.4f}")
plot_surface(e, uf, gw, labels=["E", "UF", "GW"])
"""