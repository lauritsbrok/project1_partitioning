#include "utils.h"
#include "data_tuple.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Calculate number of partitions given number of hash bits (2^hashbits)
int calculate_number_of_partitions(int hash_bits) {
    return 1 << hash_bits; // Equivalent to 2^hash_bits
}

// Generate random data tuples
DataTuple* generate_data(size_t count) {
    DataTuple* data = (DataTuple*)malloc(count * sizeof(DataTuple));
    
    // Seed the random number generator
    srand(time(NULL));
    
    for (size_t i = 0; i < count; i++) {
        // Generate random 64-bit integers for key and payload
        // Note: This is a simplification as C rand() only returns 15-31 bits
        int64_t key = ((int64_t)rand() << 32) | rand();
        int64_t payload = ((int64_t)rand() << 32) | rand();
        
        data[i].key = key;
        data[i].payload = payload;
    }
    
    return data;
}

// Save data tuples to a binary file
void save_data(DataTuple* data, size_t count, const char* path) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }
    
    // Write count
    fwrite(&count, sizeof(size_t), 1, file);
    
    // Write tuples
    for (size_t i = 0; i < count; i++) {
        fwrite(&data[i].key, sizeof(int64_t), 1, file);
        fwrite(&data[i].payload, sizeof(int64_t), 1, file);
    }
    
    fclose(file);
    printf("Data generated and saved to %s\n\n", path);
}

// Load data tuples from a binary file or generate if file doesn't exist
DataTuple* load_data(const char* path, size_t count, size_t* actual_count) {
    FILE* file = fopen(path, "rb");
    
    if (!file) {
        printf("File does not exist. Generating data...\n");
        DataTuple* data = generate_data(count);
        save_data(data, count, path);
        *actual_count = count;
        return data;
    }
    
    // Read count
    size_t file_count;
    fread(&file_count, sizeof(size_t), 1, file);
    *actual_count = file_count;
    
    // Allocate memory for data
    DataTuple* data = (DataTuple*)malloc(file_count * sizeof(DataTuple));
    
    // Read tuples
    for (size_t i = 0; i < file_count; i++) {
        fread(&data[i].key, sizeof(int64_t), 1, file);
        fread(&data[i].payload, sizeof(int64_t), 1, file);
    }
    
    fclose(file);
    printf("Data loaded from %s\n\n", path);
    
    return data;
}