using System;
using System.Collections.Concurrent;
using project1_partitioning.model;
using project1_partitioning.partition;
using project1_partitioning.Utilities;

namespace project1_partitioning.partition
{
    /// <summary>
    /// Implements the concurrent output partitioning technique.
    /// Uses a single shared buffer per partition with atomic operations for thread coordination.
    /// </summary>
    public class ConcurrentOutputPartitioner : IPartitioner
    {
        public PartitionResult Partition(DataTuple[] data, int numberOfHashBits, int numberOfThreads)
        {
            int numberOfPartitions = Utils.CalculateNumberOfPartitions(numberOfHashBits);
            var partitions = new ConcurrentDictionary<int, List<DataTuple>>();

            for (int i = 0; i < numberOfPartitions; i++)
            {
                partitions[i] = [];
            }

            Parallel.ForEach(data, new ParallelOptions { MaxDegreeOfParallelism = numberOfThreads }, tuple =>
            {
                int partitionIndex = GetPartitionIndex(tuple, numberOfHashBits);
                lock (partitions[partitionIndex])
                {
                    partitions[partitionIndex].Add(tuple);
                }
            });
            
            // Check with normal dictionary as well
            // Does this conversion hurt performance?
            return new PartitionResult { Partitions = partitions.ToDictionary(kvp => kvp.Key, kvp => kvp.Value) };
        }

        private static int GetPartitionIndex(DataTuple tuple, int numberOfHashBits)
        {
            int hash = tuple.GetHashCode();
            return Math.Abs(hash) % (int)Math.Pow(2, numberOfHashBits);
        }
    }
}
