#pragma once

#include "vec3.h"

class ray {
public:
    ray() = default;

    ray(point3 origin, vec3 direction, double time = 0.0)
        : orig { std::move(origin) }, dir { std::move(direction) },
          tm { time } {
    }

    point3 origin() const {
        return orig;
    }
    vec3 direction() const {
        return dir;
    }
    double time() const {
        return tm;
    }

    point3 at(double t) const {
        return orig + t * dir;
    }

private:
    point3 orig;
    vec3 dir;
    double tm;
};
