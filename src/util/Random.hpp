#pragma once

#include <random>

namespace util {

// Thread-local random number generator using Mersenne Twister
inline std::mt19937& getRng() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    return rng;
}

// Generate random int in range [min, max] (inclusive)
inline int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getRng());
}

// Generate random float in range [min, max)
inline float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(getRng());
}

// Generate random bool with given probability of true (0.0 to 1.0)
inline bool randomChance(float probability) {
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(getRng()) < probability;
}

} // namespace util
