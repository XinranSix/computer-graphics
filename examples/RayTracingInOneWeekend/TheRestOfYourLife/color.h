#pragma once

#include <cmath>
#include <ostream>

#include "vec3.h"
#include "interval.h"

using color = vec3;

inline double linear_to_gamma(double linear_component) {
    return std::sqrt(linear_component);
}

inline void write_color(std::ostream &os, color pixel_color,
                        int samples_per_pixel) {

    auto r { pixel_color.x() };
    auto g { pixel_color.y() };
    auto b { pixel_color.z() };

    // Replace Nan components with zero.
    if (r != r) {
        r = 0.0;
    }
    if (g != g) {
        g = 0.0;
    }
    if (b != b) {
        b = 0.0;
    }

    // Divide the color by the number of samples.
    auto scale { 1.0 / samples_per_pixel };
    r *= scale;
    g *= scale;
    b *= scale;

    // Apply the linear to gamma transform.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the tramslated [0, 255] value of each color component.
    static const interval intensity { 0.000, 0.999 };
    os << static_cast<int>(256 * intensity.clamp(r)) << ' '
       << static_cast<int>(256 * intensity.clamp(g)) << ' '
       << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}
