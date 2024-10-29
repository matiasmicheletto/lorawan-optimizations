from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

data = []

# Open data from csv file
with open('moga_so.csv') as f:
    data = f.read()

# Split the CSV data into a matrix of rows and columns
csv_matrix = [line.split(',') for line in data.strip().split('\n')]

# Convert strings to floats
matrix = [[float(cell) for cell in row] for row in csv_matrix]

# Extract vectors from columns 1, 2, and 3 (x, y, z)
vectors = [(row[0], row[1], row[2]) for row in matrix]

# Function to check if A dominates B
def dominates(A, B):
    better_in_at_least_one = False
    for i in range(3):  # For x, y, and z coordinates
        if A[i] > B[i]:
            return False  # A cannot dominate B if it's worse in one objective
        elif A[i] < B[i]:
            better_in_at_least_one = True  # A is better in at least one objective
    return better_in_at_least_one

# Calculate Pareto front
pareto_front = []
for i, vector in enumerate(vectors):
    is_dominated = False
    for j, other_vector in enumerate(vectors):
        if dominates(other_vector, vector):  # Check if another point dominates this one
            is_dominated = True
            break
    if not is_dominated:
        pareto_front.append(vector)

# Prepare 3D plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')


# Scatter plot of Pareto front in red
x2 = [v[0] for v in pareto_front]
y2 = [v[1] for v in pareto_front]
z2 = [v[2] for v in pareto_front]
ax.scatter(x2, y2, z2, color='red', label='Datos')

# Customize plot labels and title
ax.set_xlabel('G')
ax.set_ylabel('E')
ax.set_zlabel('U')
plt.title('Datos')

# Show legend and plot
plt.legend()
plt.show()
