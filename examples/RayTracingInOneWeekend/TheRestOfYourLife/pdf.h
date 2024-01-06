#pragma once

#include "rtweekend.h"
#include "hittable_list.h"

#include "onb.h"
#include <cmath>
#include <memory>

class pdf {
public:
    virtual ~pdf() = default;

    virtual double value(const vec3 &direction) const = 0;
    virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf {
public:
    sphere_pdf() = default;

    double value(const vec3 &direction) const override {
        return 1 / (4 * pi);
    }

    vec3 generate() const override {
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3 &w) {
        uvw.build_from_w(w);
    }

    double value(const vec3 &direction) const override {
        auto cosine_theta { dot(unit_vector(direction), uvw.w()) };
        return std::fmax(0, cosine_theta / pi);
    }

    vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

private:
    onb uvw;
};

class hittable_pdf : public pdf {
public:
    hittable_pdf(const hittable &_objects, const point3 &_origin)
        : objects { _objects }, origin { _origin } {
    }

    double value(const vec3 &direction) const override {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override {
        return objects.random(origin);
    }

private:
    const hittable &objects;
    point3 origin;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    double value(const vec3 &direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    vec3 generate() const override {
        if (random_double() < 0.5) {
            return p[0]->generate();
        } else {
            return p[1]->generate();
        }
    }

private:
    std::shared_ptr<pdf> p[2];
};
