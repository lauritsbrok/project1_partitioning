using System;
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
    }
}
