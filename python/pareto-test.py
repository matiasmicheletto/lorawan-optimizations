import random
from matplotlib import pyplot as plt

def dominates(a, b):
    return all(x >= y for x, y in zip(a, b)) and any(x > y for x, y in zip(a, b))

# Generate a set of random vectors
vectors = []
for i in range(100):
    vectors.append([random.random() for i in range(2)])

pareto_front = []
for i, a in enumerate(vectors):
    for j, b in enumerate(vectors):
        if i != j and dominates(a, b):
            break
    else:
        pareto_front.append(a)

print("Pareto front:")
for p in pareto_front:
    print(p)


# Scatter plot of values
x = [v[0] for v in vectors]
y = [v[1] for v in vectors]
plt.scatter(x, y)


# Scatter plot of Pareto front in red
x = [v[0] for v in pareto_front]
y = [v[1] for v in pareto_front]
plt.scatter(x, y, color='red')
plt.show()

