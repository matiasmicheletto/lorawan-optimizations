# Filename: average_columns.py

def compute_averages(filename):
    col4_sum = 0  # 4th column (index 3)
    col5_sum = 0  # 5th column (index 4)
    col6_sum = 0  # 6th column (index 5)
    count = 0

    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if len(parts) < 6:
                continue  # Skip malformed lines
            col4_sum += int(parts[3])
            col5_sum += int(parts[4])
            col6_sum += float(parts[5])
            count += 1

    if count == 0:
        print("No valid data found.")
        return

    print("Averages for last three columns:")
    print(f"Column 4 (int): {col4_sum / count:.2f}")
    print(f"Column 5 (int): {col5_sum / count:.2f}")
    print(f"Column 6 (float): {col6_sum / count:.6f}")

# Example usage:
# Save your data to 'promediar.csv' and run:
# python3 average_columns.py

if __name__ == "__main__":
    compute_averages("promediar.csv")
