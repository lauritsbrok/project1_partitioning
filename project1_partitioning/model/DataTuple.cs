namespace project1_partitioning.model;

/// <summary>
/// Represents a 16-byte tuple with an 8-byte key and an 8-byte payload.
/// </summary>
public readonly struct DataTuple(long key, long payload)
{
    public long Key { get; } = key;
    public long Payload { get; } = payload;

    public int GetPartitionIndex(int numberOfHashBits)
    {
        const uint MULTIPLICATION_CONSTANT = 2654435769; // Knuth's suggested constant
        int hash = GetHashCode();

        // Convert hash to an unsigned integer (to avoid negative values)
        uint unsignedHash = (uint)hash;

        // Multiplicative hash: Take the upper 'numberOfHashBits' bits
        return (int)((unsignedHash * MULTIPLICATION_CONSTANT) >> (32 - numberOfHashBits));
    }
}
