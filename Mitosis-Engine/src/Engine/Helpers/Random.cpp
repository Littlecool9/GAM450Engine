/**
 * @file Random.cpp
 * @author josh.siegel
 * @brief // Generates random numbers (ints and floats)
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "Random.h"

namespace mts
{
    std::mt19937 Random::generator;

    void Random::Init()
    {
        generator = std::mt19937(static_cast<unsigned>(time(nullptr)));
    }

    int Random::Range(int rangeMin, int rangeMax)
    {
        std::uniform_int_distribution<int> dist(rangeMin, rangeMax);
        return dist(generator);
    }

    float Random::Range(float rangeMin, float rangeMax)
    {
        std::uniform_real_distribution<float> dist(rangeMin, rangeMax);
        return dist(generator);
    }
}  // namespace mts