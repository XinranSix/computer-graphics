#define PI 3.141592653

vec2 fixUV(in vec2 c) {
    return 3.0 * (c * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);
}

vec3 grid(vec2 uv) {
    vec3 color = vec3(0.0);
    vec2 fraction = 1.0 - 2.0 * abs(fract(uv) - 0.5);

    color = vec3(smoothstep(4.0 * fwidth(uv.x), 3.9 * fwidth(uv.x), fraction.x));
    color += vec3(smoothstep(4.0 * fwidth(uv.y), 3.9 * fwidth(uv.y), fraction.y));
    color.rg *= smoothstep(1.9 * fwidth(uv.x), 2.0 * fwidth(uv.x), abs(uv.x));
    color.gb *= smoothstep(1.9 * fwidth(uv.y), 2.0 * fwidth(uv.y), abs(uv.y));
    return color;
}

float segment(in vec2 p, in vec2 a, in vec2 b, in float w) {
    float f = 0.0;
    vec2 ba = b - a;
    vec2 pa = p - a;
    float proj = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    float d = length(proj * ba - pa);
    f = smoothstep(w, 0.95 * w, d);
    return f;
}

float func(in float x) {
    return smoothstep(0.0, 1.0, x);
}

float funcPlot(in vec2 uv) {
    float f = 0.0;
    for (float x = 0.0; x <= iResolution.x; x+= 1.0) {
        float fx = fixUV(vec2(x, 0.0)).x;
        float nfx = fixUV(vec2(x + 1.0, 0.0)).x;
        f += segment(uv, vec2(fx, func(fx)), vec2(nfx, func(nfx)), fwidth(uv.x));
    }
    return clamp(f, 0.0, 1.0);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    vec3 color = grid(uv);
    color = mix(color, vec3(1.0, 1.0, 0.0), funcPlot(uv));
    fragColor = vec4(color, 1.0);
}
