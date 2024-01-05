#pragma once

#include "rtweekend.h"

class interval {
public:
    double min, max;
    interval()
        : min { +infinity }, max { -infinity } { // Default interval is empty
    }
    interval(double _min, double _max) : min { _min }, max { _max } {
    }
    interval(const interval &a, const interval &b)
        : min { std::fmin(a.min, b.min) }, max { std::fmax(a.max, b.max) } {
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    interval expand(double delta) const {
        auto padding { delta / 2 };
        return { min - padding, max + padding };
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    static const interval empty, universe;
};

const interval interval::empty { +infinity, -infinity };
const interval interval::universe { -infinity, +infinity };

inline interval operator+(const interval &ival, double displacement) {
    return { ival.min + displacement, ival.max + displacement };
}

inline interval operator+(double displacement, const interval &ival) {
    return ival + displacement;
}
