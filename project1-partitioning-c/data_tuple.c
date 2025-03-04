#include "data_tuple.h"
#include <stdint.h>

// Jenkins hash function (alternative to C#'s GetHashCode)
static uint32_t compute_hash(int64_t key) {
    uint64_t x = (uint64_t)key;
    x = (~x) + (x << 21); // x = (x << 21) - x - 1;
    x = x ^ (x >> 24);
    x = (x + (x << 3)) + (x << 8); // x * 265
    x = x ^ (x >> 14);
    x = (x + (x << 2)) + (x << 4); // x * 21
    x = x ^ (x >> 28);
    x = x + (x << 31);
    return (uint32_t)x;
}

int get_partition_index(DataTuple* tuple, int number_of_hash_bits) {
    const uint32_t MULTIPLICATION_CONSTANT = 2654435769u; // Knuth's suggested constant
    uint32_t hash = compute_hash(tuple->key);
    
    // Multiplicative hash: Take the upper 'numberOfHashBits' bits
    return (int)((hash * MULTIPLICATION_CONSTANT) >> (32 - number_of_hash_bits));
}