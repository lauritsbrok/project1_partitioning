using System;
using project1_partitioning.model;

namespace project1_partitioning.Utilities
{
    public static class Utils
    { 
        /// <summary>
        /// Calculates the number of partitons given number of hashbits.
        /// Using the formular 2^hashbits
        /// </summary>
        /// <param name="hashBits"></param>
        /// <returns></returns>
                         
        public static int CalculateNumberOfPartitions(int hashBits)
        {
            double hashBitsDouble = Convert.ToDouble(hashBits);
            double partitions = Math.Pow(2, hashBitsDouble);
            return Convert.ToInt32(partitions);
        }
    }
}
