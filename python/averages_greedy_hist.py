import matplotlib.pyplot as plt
import numpy as np

def load_data(filename):
    col4, col5, col6 = [], [], []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if len(parts) < 6:
                continue
            col4.append(int(parts[3]))
            col5.append(int(parts[4]))
            col6.append(float(parts[5]))
    return np.array(col4), np.array(col5), np.array(col6)

def plot_histogram(data, title, xlabel):
    mean = np.mean(data)
    std = np.std(data)

    plt.hist(data, bins=10, edgecolor='black', alpha=0.7)
    plt.axvline(mean, color='red', linestyle='dashed', linewidth=1.5, label=f'Mean = {mean:.2f}')
    plt.axvline(mean + std, color='green', linestyle='dotted', linewidth=1, label=f'+1d = {mean+std:.2f}')
    plt.axvline(mean - std, color='green', linestyle='dotted', linewidth=1, label=f'-1d = {mean-std:.2f}')
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel("Frequency")
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    col4, col5, col6 = load_data("promediar.csv")

    plot_histogram(col4, "Histogram of Gateways (e.g., parameter count)", "Gateways")
    plot_histogram(col5, "Histogram of Energy", "Energy")
    plot_histogram(col6, "Histogram of UF (e.g., runtime)", "UF")
