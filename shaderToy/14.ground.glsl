#define EPSILON 0.0001
#define MAX_DIST 200.
#define MAX_ITER 200

vec2 fixUV(vec2 uv) {
    return (2. * uv - iResolution.xy) / iResolution.x;
}

float random(vec2 pos) {
    vec2 p2 = 95.1876653 * fract(pos * 10.1321513);
    return fract((p2.x + p2.y) * p2.x * p2.y);
}

vec3 noise(vec2 pos) {
    vec2 i = floor(pos);
    vec2 f = fract(pos);
    vec2 u = f * f * (3.0 - 2.0 * f);
    vec2 du = 6. * u * (1. - u);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    return vec3(a + (b - a) * u.x * (1. - u.y) +
        (c - a) * (1. - u.x) * u.y +
        (d - a) * u.x * u.y, du * (vec2(b - a, c - a) +
        (a - b - c + d) * u.yx));
}

mat2 mat = mat2(0.6, -0.8, 0.8, 0.6);

float ground(vec2 x) {
    vec2 p = 0.003 * x;
    float a = 0.;
    float b = 1.;
    vec2 d = vec2(0);

    for(int i = 0; i < 8; i++) {
        vec3 n = noise(p);
        d += n.yz;
        a += b * n.x / (1. + dot(d, d));
        p = mat * p * 2.;
        b *= 0.5;
    }

    return 120. * a;
}

float groundH(vec2 x) {
    vec2 p = 0.003 * x;
    float a = 0.;
    float b = 1.;
    vec2 d = vec2(0);

    for(int i = 0; i < 12; i++) {
        vec3 n = noise(p);
        d += n.yz;
        a += b * n.x / (1. + dot(d, d));
        p = mat * p * 2.;
        b *= 0.5;
    }

    return 120. * a;
}

float groundL(vec2 x) {
    vec2 p = 0.003 * x;
    float a = 0.;
    float b = 1.;
    vec2 d = vec2(0);

    for(int i = 0; i < 3; i++) {
        vec3 n = noise(p);
        d += n.yz;
        a += b * n.x / (1. + dot(d, d));
        p = mat * p * 2.;
        b *= 0.5;
    }

    return 120. * a;
}

float fbm(vec2 p) {
    float f = 0.0;
    float fac = 0.5;
    for(int i = 0; i < 4; i++) {
        f += fac * noise(p).x;
        p = mat * p * 2.0;
        fac *= 0.5;
    }
    return f;
}

float rayMarch(vec3 ro, vec3 rd, float tmin, float tmax) {
    float t = tmin;
    for(int i = 0; i < MAX_ITER; i++) {
        vec3 p = ro + t * rd;
        float h = p.y - ground(p.xz);
        if(abs(h) < EPSILON * t || t > tmax)
            break;
        t += 0.3 * h;
    }
    return t;
}

float softShadow(in vec3 ro, in vec3 rd, float dis) {
    float minStep = clamp(0.01 * dis, 0.5, 50.0);
    float res = 1.0;
    float t = 0.001;
    for(int i = 0; i < 80; i++) {
        vec3 p = ro + t * rd;
        float h = p.y - ground(p.xz);
        res = min(res, 8.0 * h / t);
        t += max(minStep, h);
        if(res < 0.001 || p.y > 200.0)
            break;
    }
    return clamp(res, 0.0, 1.0);
}

vec3 calcNorm(vec3 p, float t) {
    vec2 epsilon = vec2(0.0027 * t, 0);
    return normalize(vec3(groundH(p.xz - epsilon.xy) - groundH(p.xz + epsilon.xy), 2.0 * epsilon.x, groundH(p.xz - epsilon.yx) - groundH(p.xz + epsilon.yx)));
}

mat3 setCamera(vec3 ro, vec3 target, float cr) {
    vec3 z = normalize(target - ro);
    vec3 up = normalize(vec3(sin(cr), cos(cr), 0));
    vec3 x = cross(z, up);
    vec3 y = cross(x, z);
    return mat3(x, y, z);
}

vec3 render(vec2 uv) {
    vec3 col = vec3(0);

    float an = iTime * 0.04;
    float r = 300.;
    vec2 pos2d = vec2(r * sin(an), r * cos(an));
    float h = groundL(pos2d) + 25.;
    vec3 ro = vec3(pos2d.x, h, pos2d.y);
    vec3 target = vec3(r * sin(an + 0.01), h, r * cos(an + 0.01));
    mat3 cam = setCamera(ro, target, 0.);

    float fl = 1.;
    vec3 rd = normalize(cam * vec3(uv, fl));

    float tmin = 0.001;
    float tmax = 1000.;

    float maxh = 300.;

    float tp = (maxh - ro.y) / rd.y;
    if(tp > 0.) {
        if(maxh > ro.y)
            tmax = min(tmax, tp);
        else
            tmin = max(tmin, tp);
    }
    float t = rayMarch(ro, rd, tmin, tmax);
    vec3 sunlight = normalize(vec3(0.8, 0.4, -0.2));
    float sundot = clamp(dot(rd, sunlight), 0., 1.);

    if(t > tmax) {
    // if (true) {
        // sky
        col = vec3(0.3, 0.5, 0.85) - rd.y * rd.y * 0.5;
        col = mix(col, 0.85 * vec3(0.7, 0.75, 0.85), pow(1.0 - max(rd.y, 0.0), 4.0));

        // sun
        col += 0.25 * vec3(1.0, 0.7, 0.4) * pow(sundot, 5.0);
        col += 0.25 * vec3(1.0, 0.8, 0.6) * pow(sundot, 64.0);
        col += 0.2 * vec3(1.0, 0.8, 0.6) * pow(sundot, 512.0);

        // clouds
        vec2 skyPos = ro.xz + rd.xz * (200. - ro.y) / rd.y + iTime * 5.;
        col = mix(col, vec3(1.0, 0.95, 1.0), 0.5 * smoothstep(0.4, 0.8, fbm(0.01 * skyPos)));
    } else {
        vec3 p = ro + t * rd;
        vec3 n = calcNorm(p, t);
        vec3 difColor = mix(vec3(0.08, 0.05, 0.03), vec3(0.10, 0.09, 0.08), noise(p.xz * 0.02).x);
        float r = noise(p.xz * 0.1).x;

        // rocks
        col = (r * 0.25 + 0.75) * 0.9 * difColor;
        col = mix(col, vec3(0.09, 0.06, 0.03) * (0.5 + 0.5 * r), smoothstep(0.7, 0.9, n.y));
        col = mix(col, vec3(0.045, 0.045, 0.015) * (0.25 + 0.75 * r), smoothstep(0.95, 1., n.y));
        col *= 0.1 + 1.8 * sqrt(fbm(p.xz * 0.04) * fbm(p.xz * 0.005));

        // Snow
        float h = smoothstep(35.0, 80.0, p.y + 35.0 * fbm(0.01 * p.xz));
        float e = smoothstep(1.0 - 0.5 * h, 1.0 - 0.1 * h, n.y);
        float o = 0.3 + 0.7 * smoothstep(0.0, 0.1, n.y + h * h);
        float s = h * e * o;
        col = mix(col, 0.29 * vec3(0.62, 0.65, 0.7), smoothstep(0.1, 0.9, s));

        // Linear Lighting
        vec3 lin = vec3(0.);

        float dif = clamp(dot(sunlight, n), 0., 1.);
        float sh = softShadow(p + 0.01 * sunlight, sunlight, t);
        float amb = clamp(0.5 + 0.5 * n.y, 0., 1.);
        float bac = clamp(0.2 + 0.8 * dot(vec3(-sunlight.x, 0., sunlight.z), n), 0., 1.);
        lin += dif * vec3(8.0, 5.0, 3.0) * 1.8 * vec3(sh, sh * sh * 0.5 + 0.5 * sh, sh * sh * 0.8 + 0.2 * sh);
        lin += amb * vec3(0.4, 0.6, 1.) * 1.2;
        lin += bac * vec3(0.4, 0.5, 0.6);

        col *= lin;

        // half-angle
        vec3 hal = normalize(sunlight - rd);

        col += (0.7 + 0.3 * s) * (0.04 + 0.96 * pow(clamp(1.0 + dot(hal, rd), 0.0, 1.0), 5.0)) *
            vec3(7.0, 5.0, 3.0) * sh * dif *
            pow(clamp(dot(n, hal), 0.0, 1.0), 16.0);

        col = mix(col, 0.65 * vec3(0.5, 0.75, 1.0), 1. - exp(-pow(0.002 * t, 1.5)));
    }

    // sun scatter
    col += 0.3 * vec3(1.0, 0.7, 0.3) * pow(sundot, 8.0);

    return col;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fixUV(fragCoord);
    vec3 col = render(uv);
    fragColor = vec4(1. - exp(-col * 2.), 1.);
}
