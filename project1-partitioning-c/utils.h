#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include "data_tuple.h"

// Calculate number of partitions given number of hash bits (2^hashbits)
int calculate_number_of_partitions(int hash_bits);

// Generate random data tuples
DataTuple* generate_data(size_t count);

// Save data tuples to a binary file
void save_data(DataTuple* data, size_t count, const char* path);

// Load data tuples from a binary file or generate if file doesn't exist
DataTuple* load_data(const char* path, size_t count, size_t* actual_count);

#endif // UTILS_H