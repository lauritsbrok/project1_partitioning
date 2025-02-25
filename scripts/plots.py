import pandas as pd
import matplotlib.pyplot as plt
import glob
import numpy as np

# Load all CSV files
csv_files = glob.glob("*.csv")  # Modify path if necessary
data_frames = [pd.read_csv(file) for file in csv_files]

# Concatenate all data and clean up formatting
combined_df = pd.concat(data_frames)
combined_df.columns = (
    combined_df.columns.str.strip()
)  # Remove unwanted spaces in column names
combined_df["Throughput(MTPS)"] = combined_df["Throughput(MTPS)"].astype(
    float
)  # Ensure numerical data
combined_df.dropna(inplace=True)  # Remove any accidental NaN values

# Compute the average throughput
average_df = (
    combined_df.groupby(["HashBits", "Threads"])
    .agg({"Throughput(MTPS)": "mean"})
    .reset_index()
)

# Plot settings
plt.figure(figsize=(10, 6))

# Define unique thread values for plotting
thread_values = sorted(average_df["Threads"].unique())
colors = ["r", "g", "b", "m", "c", "y"]
markers = ["+", "x", "*", "s", "p", "o"]

# Plot each thread line
for i, thread in enumerate(thread_values):
    subset = average_df[average_df["Threads"] == thread]
    plt.plot(
        subset["HashBits"],
        subset["Throughput(MTPS)"],
        marker=markers[i],
        linestyle="-",
        label=f"Threads: {thread}",
        color=colors[i],
    )

# Labels and title
plt.xlabel("Hash Bits")
plt.ylabel("Millions of Tuples per Second")

# Set x-axis labels to be 0, 2, 4, ..., 18
plt.xticks(np.arange(0, 20, 2))

plt.legend()
plt.grid(True)
plt.savefig("../plots/plot.png")  # Modify path if necessary
