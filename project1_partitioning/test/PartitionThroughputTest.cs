using System.Diagnostics;
using project1_partitioning.model;
using project1_partitioning.partition;
using project1_partitioning.Utilities;
namespace project1_partitioning.test;

public class PartitionThroughputTest
{
    /// <summary>
    /// Add documentation
    /// </summary>
    public static void ConcurrentOutputTest()
    {
        int[] hashBits = [.. Enumerable.Range(1, 18)];
        int[] threadCounts = [1, 2, 4, 8, 16, 32];      // Varying thread counts
        int tupleCount = 1_000_000;                     // 1 million tuples per test

        var partitioner = new ConcurrentOutputPartitioner();
        DataTuple[] data = DataGenerator.GenerateData((int)Math.Pow(2.0, 24.0));

        Console.WriteLine("HashBits Threads Throughput (MTPS)");

        foreach (var hashBit in hashBits)
        {
            foreach (var threads in threadCounts)
            {
                var stopwatch = Stopwatch.StartNew();
                partitioner.Partition(data, hashBit, threads);
                stopwatch.Stop();
                
                double seconds = stopwatch.Elapsed.TotalSeconds;
                double mtps = (tupleCount / 1_000_000.0) / seconds;
                
                PrintData(hashBit, threads, mtps);
            }   
        }
    }

    public static void IndependentOutputTest()
    {
        int[] hashBits = [.. Enumerable.Range(1, 18)];
        int[] threadCounts = [1, 2, 4, 8, 16, 32];      // Varying thread counts
        int tupleCount = 1_000_000;                     // 1 million tuples per test

        DataTuple[] data = DataGenerator.GenerateData((int)Math.Pow(2.0, 24.0));

        Console.WriteLine("HashBits Threads Throughput (MTPS) ChunkSize");

        foreach (var hashBit in hashBits)
        {
            foreach (var threads in threadCounts)
            {
                var stopwatch = Stopwatch.StartNew();
                IndependentOutputPartitioner.Partition(data, hashBit, threads);
                stopwatch.Stop();
                
                double seconds = stopwatch.Elapsed.TotalSeconds;
                double mtps = tupleCount / 1_000_000.0 / seconds;
                
                PrintData(hashBit, threads, mtps);
            }    
        }
    }

    private static void PrintData(int hashBit, int threads, double mtps)
    {
        if(threads < 10 && hashBit < 10) Console.WriteLine($"       {hashBit}       {threads}       {mtps:F2}");
        else if (threads >= 10 && hashBit >= 10) Console.WriteLine($"      {hashBit}      {threads}       {mtps:F2}");
        else if (threads >= 10) Console.WriteLine($"       {hashBit}      {threads}       {mtps:F2}");
        else if (hashBit >= 10) Console.WriteLine($"      {hashBit}       {threads}       {mtps:F2}");
        else Console.WriteLine($"       {hashBit}      {threads}       {mtps:F2}");
        
    }
}
