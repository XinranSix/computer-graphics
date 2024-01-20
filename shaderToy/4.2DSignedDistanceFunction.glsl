#define r 0.5 + 2.0 * sin(iTime)

vec2 fixUV(in vec2 c) {
    return (c * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);
}

float sdfCircle(in vec2 p) {
    return length(p) - (r);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    float d = sdfCircle(uv); 
    vec3 color = 1.0 - sign(d) * vec3(0.2, 0.33, 0.46);
    color *= 1.0 - exp(-3.0 * abs(d));
    color *= 0.8 + 0.2 * sin(100.0 * abs(d));
    color = mix(color, vec3(1.0), smoothstep(0.005, 0.004, abs(d)));

    if (iMouse.z > 0.1) {
        vec2 m = fixUV(iMouse.xy);
        float currentDistance = abs(sdfCircle(m));
        color = mix(color, vec3(1.0, 1.0, 0.0), smoothstep(0.01, 0.00, abs(length(uv - m) - currentDistance)));
        color = mix(color, vec3(1.0, 1.0, 0.0), smoothstep(0.02, 0.01, length(uv - m)));
    }

    fragColor = vec4(color, 1.0);
}
