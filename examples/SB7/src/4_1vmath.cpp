#include <iostream>

#include "vmath.h"

int main(int argc, char *argv[]) {

    vmath::vec3 vVertex1 { 0.0f, 0.0f, 1.0f };
    vmath::vec4 vVertex2 { vmath::vec4 { 1.0f, 0.0f, 1.0f, 1.0f } };
    vmath::vec4 vVertex3 { vVertex1, 1.0f };

    vmath::vec3 vVerts[] {
        vmath::vec3 { -0.5f, 0.0f, 0.0f },
        vmath::vec3 { 0.5f, 0.0f, 0.0f },
        vmath::vec3 { -0.0f, 0.5f, 0.0f },
    };

    vmath::vec3 a { 1.0f, 2.0f, 3.0f };
    vmath::vec3 b { 4.0f, 5.0f, 6.0f };

    auto c = a + b;
    c = a - b;
    c += b;
    c = -c;

    float d { vmath::dot(a, b) };

    return 0;
}
