#include "throughput_test.h"
#include "partitioner.h"
#include "utils.h"
#include "data_tuple.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

// Constants
#define MAX_PATH_LENGTH 256
#define TUPLE_COUNT 1000000  // 1 million tuples per test
#define ITERATIONS 1

// Partitioner type
#define CONCURRENT_OUTPUT_PARTITIONER 1
#define INDEPENDENT_OUTPUT_PARTITIONER 2

// Function to create directory if it doesn't exist
void create_directory(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        int result = mkdir(path, 0700);
        if (result != 0 && errno != EEXIST) {
            perror("Error creating directory");
            exit(1);
        }
    }
}

// Get current timestamp as string
void get_timestamp(char* buffer, size_t size) {
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, size, "%Y%m%d_%H%M%S", timeinfo);
}

// Print formatted data
void print_data(int hash_bit, int threads, double mtps) {
    if (threads < 10 && hash_bit < 10) 
        printf("       %d       %d       %.2f\n", hash_bit, threads, mtps);
    else if (threads >= 10 && hash_bit >= 10) 
        printf("      %d      %d       %.2f\n", hash_bit, threads, mtps);
    else if (threads >= 10) 
        printf("       %d      %d       %.2f\n", hash_bit, threads, mtps);
    else if (hash_bit >= 10) 
        printf("      %d       %d       %.2f\n", hash_bit, threads, mtps);
    else 
        printf("       %d      %d       %.2f\n", hash_bit, threads, mtps);
}

// Run throughput test
void run_test(const char* file_name, const char* folder_prefix, int partitioner_type) {
    // Define test parameters
    int hash_bits[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
    int thread_counts[] = {1, 2, 4, 8, 16, 32};
    int hash_bits_count = sizeof(hash_bits) / sizeof(hash_bits[0]);
    int thread_counts_count = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    // Create output directory
    char output_dir[MAX_PATH_LENGTH];
    sprintf(output_dir, "output");
    create_directory(output_dir);
    
    // Create timestamp
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Create folder name
    char folder_name[MAX_PATH_LENGTH];
    sprintf(folder_name, "%s_%s_%s", file_name, folder_prefix, timestamp);
    
    // Create folder path
    char folder_path[MAX_PATH_LENGTH];
    sprintf(folder_path, "%s/%s", output_dir, folder_name);
    create_directory(folder_path);
    
    // Load or generate data
    char data_file_path[MAX_PATH_LENGTH];
    sprintf(data_file_path, "%s/data.bin", output_dir);
    
    size_t actual_count;
    DataTuple* data = load_data(data_file_path, 1 << 24, &actual_count); // 2^24 tuples
    
    // Run iterations
    for (int i = 1; i <= ITERATIONS; i++) {
        // Create file path
        char file_path[MAX_PATH_LENGTH];
        sprintf(file_path, "%s/%s_%d.csv", folder_path, file_name, i);
        
        // Open file for writing
        FILE* file = fopen(file_path, "w");
        if (!file) {
            perror("Error opening file for writing");
            continue;
        }
        
        // Write header
        fprintf(file, "HashBits,Threads,Throughput (MTPS)\n");
        printf("HashBits,Threads,Throughput (MTPS)\n");
        
        // Run tests
        for (int h = 0; h < hash_bits_count; h++) {
            int hash_bit = hash_bits[h];
            
            for (int t = 0; t < thread_counts_count; t++) {
                int threads = thread_counts[t];
                
                // Create partition result
                PartitionResult* result = create_partition_result(calculate_number_of_partitions(hash_bit));
                
                // Measure time
                clock_t start = clock();
                
                // Run partitioning
                if (partitioner_type == CONCURRENT_OUTPUT_PARTITIONER) {
                    concurrent_output_partition(data, TUPLE_COUNT, hash_bit, threads, result);
                } else {
                    independent_output_partition(data, TUPLE_COUNT, hash_bit, threads, result);
                }
                
                clock_t end = clock();
                
                // Calculate throughput
                double seconds = (double)(end - start) / CLOCKS_PER_SEC;
                double mtps = TUPLE_COUNT / 1000000.0 / seconds;
                
                // Write to file
                fprintf(file, "%d,%d,%.6f\n", hash_bit, threads, mtps);
                print_data(hash_bit, threads, mtps);
                
                // Clean up
                free_partition_result(result);
            }
        }
        
        fclose(file);
        printf("Results saved to %s\n", file_path);
    }
    
    // Clean up
    free(data);
}

// Run concurrent output test
void run_concurrent_output_test(const char* folder_prefix) {
    run_test("ConcurrentOutputResults", folder_prefix, CONCURRENT_OUTPUT_PARTITIONER);
}

// Run independent output test
void run_independent_output_test(const char* folder_prefix) {
    run_test("IndependentOutputResults", folder_prefix, INDEPENDENT_OUTPUT_PARTITIONER);
}