#pragma once

#include "interval.h"
#include "vec3.h"
#include "ray.h"
#include <algorithm>
#include <utility>

class aabb {
public:
    interval x, y, z;

    aabb() {
    } // The default AABB is empty, since intervals are empty by default.

    aabb(interval ix, interval iy, interval iz)
        : x { std::move(ix) }, y { std::move(iy) }, z { std::move(iz) } {
    }

    aabb(const point3 &a, const point3 b) {
        // Treat the two points a and b as extrema for the bounding box, so we
        // don't require a particular minimum/maximum coordinate order.
        x = { std::min(a[0], b[0]), std::max(a[0], b[0]) };
        y = { std::min(a[1], b[1]), std::max(a[1], b[1]) };
        z = { std::min(a[2], b[2]), std::max(a[2], b[2]) };
    }

    aabb(const aabb &box0, const aabb &box1) {
        x = { box0.x, box1.x };
        y = { box0.y, box1.y };
        z = { box0.z, box1.z };
    }

    aabb pad() {
        // Return an AABB that has no side narrower than some delta, padding if
        // necessary.
        double delta { 0.0001 };
        interval new_x { x.size() >= delta ? x : x.expand(delta) };
        interval new_y { y.size() >= delta ? y : y.expand(delta) };
        interval new_z { z.size() >= delta ? z : z.expand(delta) };

        return { new_x, new_y, new_z };
    }

    const interval &axis(int n) const {
        if (n == 1) {
            return y;
        }
        if (n == 2) {
            return z;
        }
        return x;
    }

    bool hit(const ray &r, interval ray_t) const {
        for (int a {}; a < 3; ++a) {
            auto invD { 1 / r.direction()[a] };
            auto orig { r.origin()[a] };

            auto t0 { (axis(a).min - orig) * invD };
            auto t1 { (axis(a).max - orig) * invD };

            if (invD < 0) {
                std::swap(t0, t1);
            }

            if (t0 > ray_t.min) {
                ray_t.min = t0;
            }
            if (t1 < ray_t.max) {
                ray_t.max = t1;
            }

            if (ray_t.max <= ray_t.min) {
                return false;
            }
        }
        return true;
    }
};

inline aabb operator+(const aabb &bbox, const vec3 &offset) {
    return { bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z() };
}

inline aabb operator+(const vec3 &offset, const aabb &bbox) {
    return bbox + offset;
}
