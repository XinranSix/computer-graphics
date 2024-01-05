#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"
#include <cmath>
#include <memory>
#include "interval.h"

class constant_medium : public hittable {
public:
    constant_medium(std::shared_ptr<hittable> b, double d,
                    std::shared_ptr<texture> a)
        : boundary { b }, neg_inv_density { -1 / d },
          phase_function { std::make_shared<isotropic>(a) } {
    }

    constant_medium(std::shared_ptr<hittable> b, double d, color c)
        : boundary { b }, neg_inv_density { -1 / d },
          phase_function { std::make_shared<isotropic>(c) } {
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        // Print occasional samples when debugging. To enable, set enableDebug
        // true.
        const bool enableDebug {};
        const bool debugging = enableDebug && random_double() < 0.00001;

        hit_record rec1, rec2;

        // 如果一个点都没有 hit 到，返回 false
        if (!boundary->hit(r, interval::universe, rec1)) {
            return false;
        }

        // hit 到了一个点，但在这个点之后 hit 不到第二个点，可能是 hit
        // 到的那个点几乎与这个包围盒相切，此时可认为没有 hit 到，返回 false
        if (!boundary->hit(r, { rec1.t + 0.0001, infinity }, rec2)) {
            return false;
        }

        if (debugging) {
            std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t
                      << '\n';
        }

        //   调整 hit 到的时间
        if (rec1.t < ray_t.min) {
            rec1.t = ray_t.min;
        }
        if (rec2.t > ray_t.max) {
            rec2.t = ray_t.max;
        }

        // FIXME: 不懂
        if (rec1.t >= rec2.t) {
            return false;
        }

        // 光线发射点在包围盒里
        if (rec1.t < 0) {
            rec1.t = 0;
        }

        auto ray_length { r.direction().length() };
        auto distance_inside_boundary =
            (rec2.t - rec1.t) * ray_length; // 求从发射点到射出包围盒的距离
        // 随机取一个光程长度，这个长度应该和密度成反比
        auto hit_distance { neg_inv_density * std::log(random_double()) };

        // 光程大于从发射点到射出包围盒的距离，视为在包围盒内部无散射
        if (hit_distance > distance_inside_boundary) {
            return false;
        }

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " << hit_distance << '\n'
                      << "rec.t = " << rec.t << '\n'
                      << "rec.p = " << rec.p << '\n';
        }

        // 给随机方向和随机法线即可
        rec.normal = { 1, 0, 0 }; // arbitary
        rec.front_face = true;    // also arbitary
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override {
        return boundary->bounding_box();
    }

private:
    std::shared_ptr<hittable> boundary;
    double neg_inv_density;
    std::shared_ptr<material> phase_function;
};
