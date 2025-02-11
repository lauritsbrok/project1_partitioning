namespace project1_partitioning.model
{
    /// <summary>
    /// Represents a 16-byte tuple with an 8-byte key and an 8-byte payload.
    /// </summary>
    public readonly struct DataTuple(long key, long payload)
    {
        public long Key { get; } = key;
        public long Payload { get; } = payload;
    }
}
