using project1_partitioning.model;
using project1_partitioning.Utilities;

namespace project1_partitioning.partition;

public class IndependentOutputPartitioner : IPartitioner
{
    public void Partition(DataTuple[] data, int numberOfHashBits, int numberOfThreads)
    {
        int numberOfPartitions = Utils.CalculateNumberOfPartitions(numberOfHashBits);

        ThreadResult[] threadResults = new ThreadResult[numberOfThreads];
        for (int t = 0; t < numberOfThreads; t++)
        {
            ThreadResult threadResult = new();
            for (int partition = 0; partition < numberOfPartitions; partition++)
            {
                threadResult.Buffers[partition] = [];
            }
            threadResults[t] = threadResult;

        }
        int total = data.Length;
        int chunkSize = (total + numberOfThreads - 1) / numberOfThreads;

        Parallel.For(0, numberOfThreads, t =>
        {
            int start = t * chunkSize;
            int end = Math.Min(start + chunkSize, total);
            for (int i = start; i < end; i++)
            {
                DataTuple tuple = data[i];
                int partitionIndex = tuple.GetPartitionIndex(numberOfPartitions);
                if (!threadResults[t].Buffers.TryGetValue(partitionIndex, out List<DataTuple>? value))
                {
                    value = [];
                    threadResults[t].Buffers[partitionIndex] = value;
                }

                value.Add(tuple);
            }
        });
    }
}

public class ThreadResult
{
    public Dictionary<int, List<DataTuple>> Buffers { get; set; } = [];
}