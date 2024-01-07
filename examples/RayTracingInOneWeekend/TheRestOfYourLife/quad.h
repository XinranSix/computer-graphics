#pragma once

#include "hittable.h"
#include "hittable_list.h"
#include <cmath>
#include <memory>

class quad : public hittable {
public:
    quad(const point3 &_Q, const vec3 &_u, const vec3 &_v,
         std::shared_ptr<material> m)
        : Q { _Q }, u { _u }, v { _v }, mat { m } {
        auto n { cross(u, v) };
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        area = n.length();

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        bbox = aabb { Q, Q + u + v }.pad();
    }

    aabb bounding_box() const override {
        return bbox;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        auto denom { dot(normal, r.direction()) };

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8) {
            return false;
        }

        // Return false if the hit point parameter t is outside the ray
        // interval.
        auto t { (D - dot(normal, r.origin())) / denom };
        if (!ray_t.contains(t)) {
            return false;
        }

        // Determine the hit point lies within the planar shape using its plane
        // coordinates.
        auto intersection { r.at(t) };
        vec3 planar_hitpt_vector { intersection - Q };
        auto alpha { dot(w, cross(planar_hitpt_vector, v)) };
        auto beta { dot(w, cross(u, planar_hitpt_vector)) };

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }

        // Ray hits the 2D shape; set the rest of the hit record and return
        // true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record &rec) const {
        // Given the hit point in plane coordinates, return false if it is
        // outside the primitive, otherwise set the hit record UV coordinates
        // and return true.

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

    double pdf_value(const point3 &origin, const vec3 &v) const override {
        hit_record rec {};
        if (!this->hit({ origin, v }, { 0.001, infinity }, rec)) {
            return 0;
        }

        auto distance_squared { rec.t * rec.t * v.length_squared() };
        auto cosine { std::fabs(dot(v, rec.normal) / v.length()) };

        return distance_squared / (cosine * area);
    }

    vec3 random(const point3 &origin) const override {
        auto p = Q + (random_double() * u) + (random_double() * v);
        return p - origin;
    }

private:
    point3 Q;
    vec3 u, v;
    std::shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
    double area;
};

inline std::shared_ptr<hittable_list> box(const point3 &a, const point3 &b,
                                          std::shared_ptr<material> mat) {
    // Returns the 3D box (six sides) that contains the two opppsite vertices a
    // & b.

    auto sides { std::make_shared<hittable_list>() };

    // Construct the two opposite vertices with the minimum and maximum
    // coordinates.
    auto min { point3 { std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()),
                        std::fmin(a.z(), b.z()) } };
    auto max { point3 { std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()),
                        std::fmax(a.z(), b.z()) } };

    vec3 dx { max.x() - min.x(), 0, 0 };
    vec3 dy { 0, max.y() - min.y(), 0 };
    vec3 dz { 0, 0, max.z() - min.z() };

    sides->add(std::make_shared<quad>(point3 { min.x(), min.y(), max.z() }, dx,
                                      dy, mat)); // front
    sides->add(std::make_shared<quad>(point3 { max.x(), min.y(), max.z() }, -dz,
                                      dy, mat)); // right
    sides->add(std::make_shared<quad>(point3 { max.x(), min.y(), min.z() }, -dx,
                                      dy, mat)); // back
    sides->add(std::make_shared<quad>(point3 { min.x(), min.y(), min.z() }, dz,
                                      dy, mat)); // left
    sides->add(std::make_shared<quad>(point3 { min.x(), max.y(), max.z() }, dx,
                                      -dz, mat)); // top
    sides->add(std::make_shared<quad>(point3 { min.x(), min.y(), min.z() }, dx,
                                      dz, mat)); // bottom

    return sides;
}