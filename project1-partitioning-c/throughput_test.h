#ifndef THROUGHPUT_TEST_H
#define THROUGHPUT_TEST_H

#include <stddef.h>

// Run a concurrent output test
void run_concurrent_output_test(const char* folder_prefix);

// Run an independent output test
void run_independent_output_test(const char* folder_prefix);

// Run a throughput test with the given partitioner type
void run_test(const char* file_name, const char* folder_prefix, int partitioner_type);

#endif // THROUGHPUT_TEST_H 