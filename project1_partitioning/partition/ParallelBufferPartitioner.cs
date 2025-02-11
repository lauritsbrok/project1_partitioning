using System.Collections.Concurrent;
using project1_partitioning.model;
using project1_partitioning.Utilities;

namespace project1_partitioning.partition;

/// <summary>
/// Implements the parallel buffer partitioning technique.
/// ADD DESCRIPTION HERE
/// </summary>

public class ParallelBufferPartitioner(int chunkSize) : IPartitioner
{
    public PartitionResult Partition(DataTuple[] data, int numberOfHashBits, int numberOfThreads)
    {
        int numberOfChunks = (data.Length + chunkSize - 1) / chunkSize; // Ceiling division

        
        var chunks = new ConcurrentDictionary<int, List<DataTuple>>();

        for (var i = 0; i < numberOfChunks; i++)
        {
            chunks[i] = [];
        }
        
        int nextChunkIndex = 0;
        
        Parallel.For(0, numberOfThreads, new ParallelOptions { MaxDegreeOfParallelism = numberOfThreads }, _ =>
        {
            while (true)
            {
                int chunkIndex = Interlocked.Increment(ref nextChunkIndex) - 1;
                if (chunkIndex >= numberOfChunks)
                    break;

                int start = chunkIndex * chunkSize;
                int end = Math.Min(start + chunkSize, data.Length);

                var localBuffer = new List<DataTuple>();

                // Process the assigned chunk
                for (var i = start; i < end; i++)
                {
                    localBuffer.Add(data[i]);
                }

                // Store the chunk's data
                lock (chunks[chunkIndex])
                {
                    chunks[chunkIndex].AddRange(localBuffer);
                }
            }
        });

        return new PartitionResult { Partitions = chunks.ToDictionary(kvp => kvp.Key, kvp => kvp.Value) };
    }
}