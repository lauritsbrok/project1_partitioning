using System;
using System.IO;
using project1_partitioning.model;

namespace project1_partitioning.Utilities
{
    public static class DataGenerator
    {
        /// <summary>
        /// Generates an array of random DataTuple instances.
        /// </summary>
        /// <param name="count">Number of tuples to generate.</param>
        /// <returns>An array of DataTuple.</returns>
        public static DataTuple[] GenerateData(int count)
        {
            var data = new DataTuple[count];
            var random = new Random();
            for (int i = 0; i < count; i++)
            {
                // Using Random.NextInt64() available in .NET 6 and later.
                long key = random.NextInt64();
                long payload = random.NextInt64();
                data[i] = new DataTuple(key, payload);
            }
            return data;
        }

        /// <summary>
        /// Saves an array of DataTuple to a binary file.
        /// </summary>
        /// <param name="data">The array of DataTuple to save.</param>
        /// <param name="path">The file path to save the data.</param>
        public static void SaveData(DataTuple[] data, string path)
        {
            using (var fs = new FileStream(path, FileMode.Create, FileAccess.Write))
            using (var writer = new BinaryWriter(fs))
            {
                // Write the length of the array first
                writer.Write(data.Length);
                foreach (var tuple in data)
                {
                    writer.Write(tuple.Key);
                    writer.Write(tuple.Payload);
                }
            }
        }

       /// <summary>
        /// Loads an array of DataTuple from a binary file. 
        /// If the file does not exist, it generates the data and saves it.
        /// </summary>
        /// <param name="path">The file path to load the data from or save the generated data.</param>
        /// <param name="count">The number of tuples to generate if the file does not exist.</param>
        /// <returns>An array of DataTuple.</returns>
        public static DataTuple[] LoadData(string path, int count)
        {
            if (!File.Exists(path))
            {
                // Generate and save the data if file doesn't exist
                DataTuple[] data = GenerateData(count);
                SaveData(data, path);
                Console.WriteLine($"Data generated and saved to {path}");
                Console.WriteLine("");
                return data;
            }

            using (var fs = new FileStream(path, FileMode.Open, FileAccess.Read))
            using (var reader = new BinaryReader(fs))
            {
                int tupleCount = reader.ReadInt32();
                DataTuple[] data = new DataTuple[tupleCount];
                for (int i = 0; i < tupleCount; i++)
                {
                    long key = reader.ReadInt64();
                    long payload = reader.ReadInt64();
                    data[i] = new DataTuple(key, payload);
                }
                Console.WriteLine($"Data loaded from {path}\n");
                return data;
            }
        }
    }
}
