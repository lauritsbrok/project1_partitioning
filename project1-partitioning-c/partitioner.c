#include "partitioner.h"
#include "data_tuple.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define INITIAL_CAPACITY 1024

// Mutex array for concurrent output
pthread_mutex_t* partition_mutexes = NULL;

// Structure to pass arguments to threads
typedef struct {
    DataTuple* data;
    size_t start_index;
    size_t end_index;
    int number_of_hash_bits;
    int partition_count;
    ThreadResult* thread_result;      // For independent output
    PartitionResult* partition_result; // For concurrent output
    int thread_id;
} ThreadArgs;

// Initialize a new partition result
PartitionResult* create_partition_result(int partition_count) {
    PartitionResult* result = (PartitionResult*)malloc(sizeof(PartitionResult));
    result->partition_count = partition_count;
    result->partitions = (DataTuple**)malloc(partition_count * sizeof(DataTuple*));
    result->partition_sizes = (size_t*)calloc(partition_count, sizeof(size_t));
    result->partition_capacities = (size_t*)malloc(partition_count * sizeof(size_t));
    
    for (int i = 0; i < partition_count; i++) {
        result->partitions[i] = (DataTuple*)malloc(INITIAL_CAPACITY * sizeof(DataTuple));
        result->partition_capacities[i] = INITIAL_CAPACITY;
    }
    
    return result;
}

// Free a partition result
void free_partition_result(PartitionResult* result) {
    if (result) {
        for (int i = 0; i < result->partition_count; i++) {
            free(result->partitions[i]);
        }
        free(result->partitions);
        free(result->partition_sizes);
        free(result->partition_capacities);
        free(result);
    }
}

// Initialize a new thread result
ThreadResult* create_thread_result(int partition_count) {
    ThreadResult* result = (ThreadResult*)malloc(sizeof(ThreadResult));
    result->partition_count = partition_count;
    result->buffers = (DataTuple**)malloc(partition_count * sizeof(DataTuple*));
    result->buffer_sizes = (size_t*)calloc(partition_count, sizeof(size_t));
    result->buffer_capacities = (size_t*)malloc(partition_count * sizeof(size_t));
    
    for (int i = 0; i < partition_count; i++) {
        result->buffers[i] = (DataTuple*)malloc(INITIAL_CAPACITY * sizeof(DataTuple));
        result->buffer_capacities[i] = INITIAL_CAPACITY;
    }
    
    return result;
}

// Free a thread result
void free_thread_result(ThreadResult* result) {
    if (result) {
        for (int i = 0; i < result->partition_count; i++) {
            free(result->buffers[i]);
        }
        free(result->buffers);
        free(result->buffer_sizes);
        free(result->buffer_capacities);
        free(result);
    }
}

// Add a tuple to a partition in the result
void add_tuple_to_partition(PartitionResult* result, int partition_index, DataTuple tuple) {
    if (result->partition_sizes[partition_index] >= result->partition_capacities[partition_index]) {
        // Resize the partition
        result->partition_capacities[partition_index] *= 2;
        result->partitions[partition_index] = (DataTuple*)realloc(
            result->partitions[partition_index],
            result->partition_capacities[partition_index] * sizeof(DataTuple)
        );
    }
    
    result->partitions[partition_index][result->partition_sizes[partition_index]++] = tuple;
}

// Add a tuple to a buffer in the thread result
void add_tuple_to_thread_buffer(ThreadResult* result, int partition_index, DataTuple tuple) {
    if (result->buffer_sizes[partition_index] >= result->buffer_capacities[partition_index]) {
        // Resize the buffer
        result->buffer_capacities[partition_index] *= 2;
        result->buffers[partition_index] = (DataTuple*)realloc(
            result->buffers[partition_index],
            result->buffer_capacities[partition_index] * sizeof(DataTuple)
        );
    }
    
    result->buffers[partition_index][result->buffer_sizes[partition_index]++] = tuple;
}

// Merge thread results into a partition result
void merge_thread_results(PartitionResult* partition_result, ThreadResult** thread_results, int thread_count) {
    for (int t = 0; t < thread_count; t++) {
        ThreadResult* thread_result = thread_results[t];
        
        for (int p = 0; p < partition_result->partition_count; p++) {
            size_t buffer_size = thread_result->buffer_sizes[p];
            
            // Skip empty buffers
            if (buffer_size == 0) continue;
            
            // Ensure partition has enough capacity
            while (partition_result->partition_sizes[p] + buffer_size > partition_result->partition_capacities[p]) {
                partition_result->partition_capacities[p] *= 2;
                partition_result->partitions[p] = (DataTuple*)realloc(
                    partition_result->partitions[p],
                    partition_result->partition_capacities[p] * sizeof(DataTuple)
                );
            }
            
            // Copy tuples from thread buffer to partition
            memcpy(
                &partition_result->partitions[p][partition_result->partition_sizes[p]],
                thread_result->buffers[p],
                buffer_size * sizeof(DataTuple)
            );
            
            partition_result->partition_sizes[p] += buffer_size;
        }
    }
}

// Thread function for concurrent output partitioning
void* concurrent_output_thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    
    for (size_t i = args->start_index; i < args->end_index; i++) {
        DataTuple tuple = args->data[i];
        int partition_index = get_partition_index(&tuple, args->number_of_hash_bits);
        
        // Lock the partition
        pthread_mutex_lock(&partition_mutexes[partition_index]);
        
        // Add tuple to partition
        add_tuple_to_partition(args->partition_result, partition_index, tuple);
        
        // Unlock the partition
        pthread_mutex_unlock(&partition_mutexes[partition_index]);
    }
    
    return NULL;
}

// Thread function for independent output partitioning
void* independent_output_thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    
    for (size_t i = args->start_index; i < args->end_index; i++) {
        DataTuple tuple = args->data[i];
        int partition_index = get_partition_index(&tuple, args->number_of_hash_bits);
        
        // Add tuple to thread-local buffer
        add_tuple_to_thread_buffer(args->thread_result, partition_index, tuple);
    }
    
    return NULL;
}

// Concurrent output partitioning implementation
void concurrent_output_partition(DataTuple* data, size_t data_length, int number_of_hash_bits, int number_of_threads, PartitionResult* result) {
    int partition_count = calculate_number_of_partitions(number_of_hash_bits);
    
    // Initialize mutexes
    partition_mutexes = (pthread_mutex_t*)malloc(partition_count * sizeof(pthread_mutex_t));
    for (int i = 0; i < partition_count; i++) {
        pthread_mutex_init(&partition_mutexes[i], NULL);
    }
    
    // Create threads and arguments
    pthread_t* threads = (pthread_t*)malloc(number_of_threads * sizeof(pthread_t));
    ThreadArgs* thread_args = (ThreadArgs*)malloc(number_of_threads * sizeof(ThreadArgs));
    
    size_t chunk_size = (data_length + number_of_threads - 1) / number_of_threads;
    
    // Create and start threads
    for (int t = 0; t < number_of_threads; t++) {
        size_t start_index = t * chunk_size;
        size_t end_index = start_index + chunk_size;
        if (end_index > data_length) end_index = data_length;
        
        thread_args[t].data = data;
        thread_args[t].start_index = start_index;
        thread_args[t].end_index = end_index;
        thread_args[t].number_of_hash_bits = number_of_hash_bits;
        thread_args[t].partition_count = partition_count;
        thread_args[t].partition_result = result;
        thread_args[t].thread_id = t;
        
        pthread_create(&threads[t], NULL, concurrent_output_thread_func, &thread_args[t]);
    }
    
    // Wait for all threads to complete
    for (int t = 0; t < number_of_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    // Clean up
    free(threads);
    free(thread_args);
    
    for (int i = 0; i < partition_count; i++) {
        pthread_mutex_destroy(&partition_mutexes[i]);
    }
    free(partition_mutexes);
}

// Independent output partitioning implementation
void independent_output_partition(DataTuple* data, size_t data_length, int number_of_hash_bits, int number_of_threads, PartitionResult* result) {
    int partition_count = calculate_number_of_partitions(number_of_hash_bits);
    
    // Create threads and arguments
    pthread_t* threads = (pthread_t*)malloc(number_of_threads * sizeof(pthread_t));
    ThreadArgs* thread_args = (ThreadArgs*)malloc(number_of_threads * sizeof(ThreadArgs));
    
    // Create thread results
    ThreadResult** thread_results = (ThreadResult**)malloc(number_of_threads * sizeof(ThreadResult*));
    for (int t = 0; t < number_of_threads; t++) {
        thread_results[t] = create_thread_result(partition_count);
    }
    
    size_t chunk_size = (data_length + number_of_threads - 1) / number_of_threads;
    
    // Create and start threads
    for (int t = 0; t < number_of_threads; t++) {
        size_t start_index = t * chunk_size;
        size_t end_index = start_index + chunk_size;
        if (end_index > data_length) end_index = data_length;
        
        thread_args[t].data = data;
        thread_args[t].start_index = start_index;
        thread_args[t].end_index = end_index;
        thread_args[t].number_of_hash_bits = number_of_hash_bits;
        thread_args[t].partition_count = partition_count;
        thread_args[t].thread_result = thread_results[t];
        thread_args[t].thread_id = t;
        
        pthread_create(&threads[t], NULL, independent_output_thread_func, &thread_args[t]);
    }
    
    // Wait for all threads to complete
    for (int t = 0; t < number_of_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    // Merge thread results into the final result
    merge_thread_results(result, thread_results, number_of_threads);
    
    // Clean up
    for (int t = 0; t < number_of_threads; t++) {
        free_thread_result(thread_results[t]);
    }
    free(thread_results);
    free(threads);
    free(thread_args);
}