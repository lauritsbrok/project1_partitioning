using project1_partitioning.test;

class Program
{
    static void Main(string[] args)
    {
        if (args.Length == 0)
        {
            Console.WriteLine("Error: Missing folder name prefix. Please provide it as the first argument.");
            Environment.Exit(1); // Exit with a non-zero status to indicate an error
        }

        string folderPrefix = args[0];

        PartitionThroughputTest.ConcurrentOutputTest(folderPrefix);
        PartitionThroughputTest.IndependentOutputTest(folderPrefix);
    }
}