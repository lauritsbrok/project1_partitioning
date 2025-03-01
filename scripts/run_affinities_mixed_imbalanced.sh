#!/bin/bash

echo "Building release..."

# Move to the project directory to build
cd ../project1_partitioning || exit 1

# Build in Release mode
dotnet build -c Release

# Go back to the script's directory
cd - > /dev/null

echo ""

# Define multiple core sets
CORE_SETS=("0,1,2,3,4,6,8,10,11")

# Run for each core set
for CORES in "${CORE_SETS[@]}"; do
    echo "Running on cores: $CORES"
    ARG="core_$CORES"
    taskset -c $CORES ./project1_partitioning/bin/Release/net9.0/project1_partitioning "$ARG"
    echo "Finished test on cores: $CORES"
    echo "-------------------------"
    echo ""
done