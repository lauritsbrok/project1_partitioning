namespace project1_partitioning.model
{
    /// <summary>
    /// Represents the result of partitioning: a mapping of partition indices to lists of tuples.
    /// </summary>
    public class PartitionResult
    {
        public Dictionary<int, List<DataTuple>> Partitions { get; set; } = [];
    }
}