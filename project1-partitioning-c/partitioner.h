#ifndef PARTITIONER_H
#define PARTITIONER_H

#include "data_tuple.h"
#include <stddef.h>

// Forward declarations
typedef struct PartitionResult PartitionResult;
typedef struct ThreadResult ThreadResult;

// Structure to hold partition results
struct PartitionResult {
    DataTuple** partitions;  // Array of arrays of DataTuple
    size_t* partition_sizes; // Size of each partition
    size_t* partition_capacities; // Capacity of each partition
    int partition_count;
};

// Structure to hold thread-local results for independent output
struct ThreadResult {
    DataTuple** buffers;      // Array of arrays of DataTuple
    size_t* buffer_sizes;     // Size of each buffer
    size_t* buffer_capacities; // Capacity of each buffer
    int partition_count;
};

// Initialize a new partition result
PartitionResult* create_partition_result(int partition_count);

// Free a partition result
void free_partition_result(PartitionResult* result);

// Initialize a new thread result
ThreadResult* create_thread_result(int partition_count);

// Free a thread result
void free_thread_result(ThreadResult* result);

// Add a tuple to a partition in the result
void add_tuple_to_partition(PartitionResult* result, int partition_index, DataTuple tuple);

// Add a tuple to a buffer in the thread result
void add_tuple_to_thread_buffer(ThreadResult* result, int partition_index, DataTuple tuple);

// Merge thread results into a partition result
void merge_thread_results(PartitionResult* partition_result, ThreadResult** thread_results, int thread_count);

// Function prototypes for partitioning
void concurrent_output_partition(DataTuple* data, size_t data_length, int number_of_hash_bits, int number_of_threads, PartitionResult* result);
void independent_output_partition(DataTuple* data, size_t data_length, int number_of_hash_bits, int number_of_threads, PartitionResult* result);

#endif // PARTITIONER_H