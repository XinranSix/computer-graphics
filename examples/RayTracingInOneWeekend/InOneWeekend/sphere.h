#pragma once

#include "hittable.h"
#include "vec3.h"
#include <memory>

class sphere : public hittable {
public:
    sphere(point3 _cenetr, double _radius, std::shared_ptr<material> _material)
        : center { _cenetr }, radius { _radius }, mat { _material } {};

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        vec3 oc { r.origin() - center };
        auto a { r.direction().length_squared() };
        auto half_b = dot(oc, r.direction());
        auto c { oc.length_squared() - radius * radius };

        auto direction = half_b * half_b - a * c;
        if (direction < 0) {
            return false;
        }
        auto sqrtd { std::sqrt(direction) };

        // Find the nearest root that lies in the acceptabl range.
        auto root { (-half_b - sqrtd) / a };
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal { (rec.p - center) / radius };
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    point3 center;
    double radius;
    std::shared_ptr<material> mat;
};
