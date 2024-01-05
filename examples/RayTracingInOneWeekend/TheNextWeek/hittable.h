#pragma once

#include "ray.h"
#include "interval.h"
#include "aabb.h"

#include <cmath>
#include <memory>

class material;

class hit_record {
public:
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    void set_face_normal(const ray &r, const vec3 &outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;
    virtual aabb bounding_box() const = 0;
};

class translate : public hittable {
public:
    translate(std::shared_ptr<hittable> p, const vec3 &displacement)
        : object { p }, offset { displacement } {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        // Move the ray backward by the offset
        ray offset_r { r.origin() - offset, r.direction(), r.time() };

        // Determine where (if any) an intersection occurs along the offset ray
        if (!object->hit(offset_r, ray_t, rec)) {
            return false;
        }

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    aabb bounding_box() const override {
        return bbox;
    }

private:
    std::shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

class rotate_y : public hittable {
public:
    rotate_y(std::shared_ptr<hittable> p, double angle) : object { p } {
        auto radius { degrees_to_radians(angle) };
        sin_theta = std::sin(radius);
        cos_theta = std::cos(radius);
        bbox = object->bounding_box();

        point3 min { infinity, infinity, infinity };
        point3 max { -infinity, -infinity, -infinity };

        for (int i {}; i < 2; ++i) {
            for (int j {}; j < 2; ++j) {
                for (int k {}; k < 2; ++k) {
                    auto x { i * bbox.x.max + (1 - i) * bbox.x.min };
                    auto y { j * bbox.y.max + (1 - j) * bbox.y.min };
                    auto z { k * bbox.z.max + (1 - k) * bbox.z.min };

                    auto newx { cos_theta * x + sin_theta * z };
                    auto newz { -sin_theta * x + cos_theta * z };

                    vec3 tester { newx, y, newz };

                    for (int c {}; c < 3; ++c) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = { min, max };
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        // Change the ray from world space to object space
        auto origin { r.origin() };
        auto direction { r.direction() };

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] =
            cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] =
            sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        ray rotated_r { origin, direction, r.time() };

        // Determine where (if any) an intersection occurs in object space
        if (!object->hit(rotated_r, ray_t, rec)) {
            return false;
        }

        // Change the intersection from object to world space
        auto p { rec.p };
        p[0] = cos_theta * p[0] + sin_theta * p[2];
        p[2] = -sin_theta * p[0] + cos_theta * p[2];

        // Change the normal from object to world space
        auto normal { rec.normal };
        normal[0] = cos_theta * normal[0] + sin_theta * normal[2];
        normal[2] = -sin_theta * normal[0] + cos_theta * normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override {
        return bbox;
    }

private:
    std::shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};
