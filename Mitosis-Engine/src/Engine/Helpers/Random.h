/**
 * @file Random.h
 * @author josh.siegel
 * @brief // Generates random numbers (ints and floats)
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <random>

namespace mts
{
    class Random
    {
       public:
        static void Init();

        // Generate random int in [rangeMin, rangeMax]
        static int Range(int rangeMin, int rangeMax);

        // Generate random float in [rangeMin, rangeMax)
        static float Range(float rangeMin, float rangeMax);

       private:
        static std::mt19937 generator;
    };

}  // namespace mts