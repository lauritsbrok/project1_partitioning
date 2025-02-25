using System.Diagnostics;
using project1_partitioning.model;
using project1_partitioning.partition;
using project1_partitioning.Utilities;
namespace project1_partitioning.test;

public class PartitionThroughputTest
{
    /// <summary>
    /// Runs a throughput test with the given partitioner factory and writes the results to a CSV file.
    /// </summary>
    /// <param name="filePath">The CSV file to write results to.</param>
    /// <param name="partitionerFactory">A function that creates an instance of a partitioner.</param>
    public static void RunTest(string fileName, string folderPrefix, Func<IPartitioner> partitionerFactory)
    {
        //int[] hashBits = Enumerable.Range(1, 2).ToArray();
        int[] hashBits = [.. Enumerable.Range(3, 3)];
        int[] threadCounts = [1, 2, 4, 8, 16, 32];
        int tupleCount = 1_000_000; // 1 million tuples per test
        int iterations = 8;

        string outputDir = Path.Combine(Directory.GetCurrentDirectory(), "output");
        if (!Directory.Exists(outputDir))
        {
            Directory.CreateDirectory(outputDir);
        }

        // Create a timestamp
        string timestamp = DateTime.Now.ToString("yyyyMMdd_HHmmss");
        string folderName = $"{fileName}_{folderPrefix}_{timestamp}";

        // Create a new directory inside output
        string folderPath = Path.Combine(outputDir, folderName);
        if (!Directory.Exists(folderPath))
        {
            Directory.CreateDirectory(folderPath);
        }

        string dataFilePath = Path.Combine(outputDir, "data.bin");
        DataTuple[] data = DataGenerator.LoadData(dataFilePath, (int)Math.Pow(2.0, 24.0));

        for (int i = 1; i <= iterations; i++)
        {
            string filePath = Path.Combine(folderPath, $"{fileName}_{i}.csv");
            // Write the header to the CSV file
            using (StreamWriter writer = new(filePath))
            {
                writer.WriteLine("HashBits,Threads,Throughput (MTPS)");
                Console.WriteLine("HashBits,Threads,Throughput (MTPS)");

                foreach (var hashBit in hashBits)
                {
                    foreach (var threads in threadCounts)
                    {
                        var partitioner = partitionerFactory();
                        var stopwatch = Stopwatch.StartNew();
                        partitioner.Partition(data, hashBit, threads);
                        stopwatch.Stop();

                        double seconds = stopwatch.Elapsed.TotalSeconds;
                        double mtps = tupleCount / 1_000_000.0 / seconds;

                        // Write result to CSV
                        writer.WriteLine($"{hashBit},{threads},{mtps:F6}");
                        PrintData(hashBit, threads, mtps);
                    }
                }
            }
            Console.WriteLine($"Results saved to {filePath}");
        }
    }

    public static void ConcurrentOutputTest(string folderPrefix)
    {
        RunTest("ConcurrentOutputResults", folderPrefix, () => new ConcurrentOutputPartitioner());
    }

    public static void IndependentOutputTest(string folderPrefix)
    {
        RunTest("IndependentOutputResults", folderPrefix, () => new IndependentOutputPartitioner());
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
