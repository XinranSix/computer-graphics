#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Constants

constexpr double infinity { std::numeric_limits<double>::infinity() };
constexpr double pi { 3.1415926535897932385 };

// Utility Funcrions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a rnadom real in [0, 1).
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min, max).
    return min + (max - min) * random_double();
}

// Common Headers
