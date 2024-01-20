#define PI 3.141592653

vec2 fixUV(in vec2 c) {
    return 3.0 * (c * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);
}

vec3 grid(vec2 uv) {
    vec3 color = vec3(0.4);
    vec2 grid = floor(mod(uv, 2.0));
    if (grid.x == grid.y) {
        color = vec3(0.6);
    }
    color = mix(color, vec3(0.0), smoothstep(1.1 * fwidth(uv.x), fwidth(uv.x), abs(uv.x)));
    color = mix(color, vec3(0.0), smoothstep(1.1 * fwidth(uv.x), fwidth(uv.y), abs(uv.y)));
    
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
    float T = 4.0 * 2.0 * cos(iTime);
    return sin(2.0 * PI / T * x);
}

float plotFun(in vec2 uv) {
    float f = func(uv.x);
    return smoothstep(f - 0.01, f + 0.01, uv.y);
}

#define AA 4

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    vec3 color = grid(uv);
    float count = 0.0;
    for (int m = 0; m < AA; m++) {
        for (int n = 0; n < AA; n++) {
            vec2 offset = (vec2(float(m), float(n)) - 0.5 * float(AA)) / float(AA) * 2.0; 
            count += plotFun(fixUV(fragCoord + offset));
        }
    }

    if (count > float(AA * AA) / 2.0) {
        count = float(AA * AA) - count;
    }
    count = count * 2.0 / float(AA * AA);
    color = mix(color, vec3(1.0), vec3(count));
    
    fragColor = vec4(color, 1.0);
}
