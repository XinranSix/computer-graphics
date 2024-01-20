#define C 0.707107

float noise(vec2 x) {
    return abs(fract(114514.114514 * sin(dot(x, vec2(123.0, 456.0)))));
}

float line(vec2 p, float dir) {
    float d = dot(p, dir > 0.5 ? vec2(C, C) : vec2(-C, C));
    return smoothstep(0.01, 0.0, abs(d));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = 5.0 * 2.0 * (fragCoord - 0.5 * iResolution.xy) / min(iResolution.x, iResolution.y);
    vec3 col = vec3(line(fract(uv) - 0.5, noise(floor(uv))));
    fragColor = vec4(col, 1.0);
}
