using project1_partitioning.model;

namespace project1_partitioning.partition;

public interface IPartitioner
{
    /// <summary>
    /// Partitions the input data using a specified partitioning technique.
    /// </summary>
    /// <param name="data">The array of input tuples.</param>
    /// <param name="numberOfHashBits">The number of hash bits used (resulting in 2^numberOfHashBits partitions).</param>
    /// <param name="numberOfThreads">The number of threads to use.</param>
    void Partition(DataTuple[] data, int numberOfHashBits, int numberOfThreads);
    
}
