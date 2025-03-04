#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "throughput_test.h"

void print_usage() {
    printf("Usage: partition_test [test_type] [prefix]\n");
    printf("  test_type: 'concurrent', 'independent', or 'both'\n");
    printf("  prefix: An optional prefix for the output folder\n");
}

int main(int argc, char* argv[]) {
    // Default values
    const char* test_type = "both";
    const char* prefix = "test";
    
    // Parse command line arguments
    if (argc > 1) {
        test_type = argv[1];
    }
    
    if (argc > 2) {
        prefix = argv[2];
    }
    
    // Run tests
    if (strcmp(test_type, "concurrent") == 0) {
        printf("Running Concurrent Output Test with prefix '%s'\n", prefix);
        run_concurrent_output_test(prefix);
    } else if (strcmp(test_type, "independent") == 0) {
        printf("Running Independent Output Test with prefix '%s'\n", prefix);
        run_independent_output_test(prefix);
    } else if (strcmp(test_type, "both") == 0) {
        printf("Running both Concurrent and Independent Output Tests with prefix '%s'\n", prefix);
        run_concurrent_output_test(prefix);
        run_independent_output_test(prefix);
    } else {
        printf("Unknown test type: %s\n", test_type);
        print_usage();
        return 1;
    }
    
    return 0;
}