#pragma once

#include "rtweekend.h"
#include "vec3.h"
#include <cmath>

class perlin {
public:
    perlin() {
        ranvec = new vec3[point_count];
        for (int i {}; i < point_count; ++i) {
            ranvec[i] = unit_vector(vec3::random(-1, 1));
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] ranvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const point3 &p) const {

        auto u { p.x() - std::floor(p.x()) };
        auto v { p.y() - std::floor(p.y()) };
        auto w { p.z() - std::floor(p.z()) };

        auto i { static_cast<int>(std::floor(p.x())) };
        auto j { static_cast<int>(std::floor(p.y())) };
        auto k { static_cast<int>(std::floor(p.z())) };

        vec3 c[2][2][2];

        for (int di {}; di < 2; ++di) {
            for (int dj {}; dj < 2; ++dj) {
                for (int dk {}; dk < 2; ++dk) {
                    c[di][dj][dk] =
                        ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                               perm_z[(k + dk) & 255]];
                }
            }
        }

        return trilinear_interp(c, u, v, w);
    }

    double turb(const point3 &p, int depth = 7) const {
        auto accum { 0.0 };
        auto temp_p { p };
        auto weight { 1.0 };

        for (int i {}; i < depth; ++i) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return std::fabs(accum);
    }

private:
    static const int point_count { 256 };
    vec3 *ranvec;
    int *perm_x;
    int *perm_y;
    int *perm_z;

    static int *perlin_generate_perm() {
        auto p = new int[point_count];

        for (int i {}; i < perlin::point_count; ++i) {
            p[i] = i;
        }

        permute(p, point_count);

        return p;
    }

    // 洗牌
    static void permute(int *p, int n) {
        for (int i { n - 1 }; i > 0; --i) {
            auto target { random_int(0, i) };
            auto tmp { p[i] };
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    // 三线性插值
    static double trilinear_interp(vec3 c[2][2][2], double u, double v,
                                   double w) {
        auto uu { u * u * (3 - 2 * u) };
        auto vv { v * v * (3 - 2 * v) };
        auto ww { w * w * (3 - 2 * w) };
        double accum {};

        for (int i {}; i < 2; ++i) {
            for (int j {}; j < 2; ++j) {
                for (int k {}; k < 2; ++k) {
                    vec3 weight_v { u - i, v - j, w - k };
                    accum += (i * uu + (1 - i) * (1 - uu)) *
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) *
                             dot(c[i][j][k], weight_v);
                }
            }
        }

        return accum;
    }
};
