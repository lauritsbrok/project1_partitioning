import pandas as pd
import matplotlib.pyplot as plt
import glob
import numpy as np
import os
import sys

# Load all CSV files
output_dir = "../plots"
os.makedirs(output_dir, exist_ok=True)  # Create directory if it doesn't exist

# Get all directories matching the pattern
directories = glob.glob("../output/*")

if not directories:
    print("No directories found matching pattern")
    sys.exit(1)

for directory in directories:
    csv_files = glob.glob(f"{directory}/*.csv")  # Get all CSV files in the directory

    if not csv_files:
        print(f"No files found in directory '{directory}'")
        continue

    data_frames = [pd.read_csv(file) for file in csv_files]

    # Concatenate all data and clean up formatting
    combined_df = pd.concat(data_frames)
    combined_df.columns = (
        combined_df.columns.str.strip()
    )  # Remove unwanted spaces in column names
    combined_df["Throughput (MTPS)"] = combined_df["Throughput (MTPS)"].astype(
        float
    )  # Ensure numerical data
    combined_df.dropna(inplace=True)  # Remove any accidental NaN values

    # Compute the average throughput
    average_df = (
        combined_df.groupby(["HashBits", "Threads"])
        .agg({"Throughput (MTPS)": "mean"})
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
            subset["Throughput (MTPS)"],
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
    # Save the plot with the directory name
    directory_name = os.path.basename(directory)
    plt.savefig(f"{output_dir}/{directory_name}.png")
    plt.close()
