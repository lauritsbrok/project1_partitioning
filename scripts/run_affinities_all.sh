#!/bin/bash

echo "Building release..."

# Move to the project directory to build
cd ../project1_partitioning || exit 1

# Build in Release mode
dotnet build -c Release

# Go back to the script's directory
cd - > /dev/null

echo ""

#Run affinities

# When assigning just core 0, you are binding the process to P#0 (which is one thread on Core L#0).
# This means only one thread of Core L#0 is active—not the full physical core.
# The OS might still allocate memory from either NUMA node, potentially causing cross-NUMA access.

# When assigning 0,2,4,6,8,10,12,14, it selects only P#s that belong to NUMA Node 0. All selected P#s
# correspond to one logical processor per physical core within NUMA Node 0. Since all execution happens
# on NUMA Node 0, the OS is much more likely to allocate memory locally, avoiding cross-NUMA penalties.

# Intuitively, if just 0 incurs a high risk of cross-NUMA access, and 0,2,4,6,8,10,12,14 is low-risk,
# then selecting just 0,2,4,6 would be moderate risk.

# Core configurations:
# 0                     (Worst case, single core, cross-NUMA risk) Runs on one core, but its memory might reside in either NUMA node, causing potential cross-NUMA latency.
# 0,16                 (Best single-core, within NUMA) Uses one physical core (L#0) but includes both hyperthreads (P#0 & P#16) to measure hyperthreading gains.
# 0,2,4,6,8,10,12,14    (Best same-NUMA, multiple cores, single NUMA, no hyperthreading) Uses all 8 physical cores from NUMA Node 0, avoiding cross-NUMA penalties.
# 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30    (Best same-NUMA, multiple cores, single NUMA, with hyperthreading) Uses all logical CPUs from NUMA Node 0, testing if hyperthreading helps or hinders performance.
# 0,2,4,6,9,11,13,15    (Cross-NUMA (Half cores from each node)) Mixes four cores per NUMA node, measuring performance when workload is split across memory controllers.
# 0,1,2,3,4,6,8,10,11   (NUMA-Imbalanced (Heavy on One, Light on the Other)) Loads NUMA Node 0 heavily while using only 3 cores from NUMA Node 1, testing imbalanced workload effects.
# 16-31                 (Hyperthreading Stress Test (Only Hyperthreads, No Primary Cores)) Forces the workload to use only hyperthreads while primary threads stay idle.
# 0-15                  (Cross-NUMA (All Physical Cores, No Hyperthreading))   Uses all 16 physical cores across both NUMA nodes.
# 0-31                  (Cross-NUMA (All Physical Cores, With Hyperthreading)) Uses all 32 logical processors, measuring the system’s full capacity.

# Why these?

# Why These?
# - Covers single-core vs multi-core
# - Tests NUMA effects (same vs. cross-node performance)
# - Compares hyperthreading benefits vs. penalties
# - Includes an imbalanced workload scenario

# Define multiple core sets
CORE_SETS=("0" "0,16" "0,2,4,6,8,10,12,14" "0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30" "0,2,4,6,9,11,13,15" "0,1,2,3,4,6,8,10,11" "16-31" "0-15" "0-31")

# Run for each core set
for CORES in "${CORE_SETS[@]}"; do
    echo "Running on cores: $CORES"
    ARG="core_$CORES"
    taskset -c $CORES ./project1_partitioning/bin/Release/net9.0/project1_partitioning "$ARG"
    echo "Finished test on cores: $CORES"
    echo "-------------------------"
    echo ""
done