#include "rtweekend.h"
#include "vec3.h"

#include <cmath>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <stdlib.h>

double f(const vec3 &d) {
    auto cosine_squared { d.z() * d.z() };
    return cosine_squared;
}

double pdf(const vec3 &d) {
    return 1 / (4 * pi);
}

int main(int argc, char *argv[]) {

    int N { 1000000 };
    double sum {};
    for (int i {}; i < N; ++i) {
        auto d { random_unit_vector() };
        auto f_d = f(d);
        sum += f_d / pdf(d);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';

    return 0;
}
