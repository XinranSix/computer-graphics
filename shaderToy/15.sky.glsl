vec2 fixUV(in vec2 uv) {
    return 2. * (uv - .5 * iResolution.xy) / iResolution.y;
}

mat3 setCamera(in vec3 ro, in vec3 target, float cr) {
    vec3 cw = normalize(target - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.0);
    vec3 cu = normalize(cross(cw, cp));
    vec3 cv = normalize(cross(cu, cw));
    return mat3(cu, cv, cw);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    vec3 target = vec3(0, 4, 0);
    float r = 10.;
    float an = iTime * 0.5;
    vec3 ro = vec3(r * sin(an), 2., r * cos(an));
    mat3 cam = setCamera(ro, target, 0.);

    float fl = 2.;
    vec3 rd = normalize(cam * vec3(uv, fl));
    vec3 col = vec3(0.3, 0.5, 0.85) - rd.y * rd.y * 0.5;
    col = mix(col, 0.85 * vec3(0.7, 0.75, 0.85), pow(1.0 - max(rd.y, 0.0), 4.0));

    // sun
    vec3 sunLight = normalize(vec3(0., 0.4, -1.0));
    float sundot = clamp(dot(rd, sunLight), 0.0, 1.0);

    col += 0.25 * vec3(1.0, 0.7, 0.4) * pow(sundot, 5.0);
    col += 0.25 * vec3(1.0, 0.8, 0.6) * pow(sundot, 64.0);
    col += 0.25 * vec3(1.0, 0.8, 0.6) * pow(sundot, 512.0);

    col = mix(col, 0.68 * vec3(0.4, 0.65, 1.0), pow(1.0 - max(rd.y, 0.0), 16.0));

    fragColor = vec4(sqrt(col), 1.);
}
