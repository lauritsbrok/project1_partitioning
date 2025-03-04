#ifndef DATA_TUPLE_H
#define DATA_TUPLE_H

#include <stdint.h>

// Equivalent to the C# DataTuple struct
typedef struct {
    int64_t key;
    int64_t payload;
} DataTuple;

// Function to get partition index for a tuple
int get_partition_index(DataTuple* tuple, int number_of_partitions);

#endif // DATA_TUPLE_H