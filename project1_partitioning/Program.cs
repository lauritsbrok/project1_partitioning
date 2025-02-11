using project1_partitioning.partition;
using project1_partitioning.test;

// PartitionThroughputTest.Test(new ConcurrentOutputPartitioner());
PartitionThroughputTest.Test(new ParallelBufferPartitioner());