#iChannel0 "file://London.jpg"
#iChannel0::WrapMode "Repeat"

#define TMIN 0.1
#define TMAX 20.
#define RAYMARCH_TIME 128
#define PRECISION .001
#define AA 3
#define PI 3.14159265

vec2 fixUV(in vec2 c) {
    return (2. * c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

float sdfRect(in vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(max(d.x, d.y),d.z), 0.);
}

float map(in vec3 p) {
    float d = sdfRect(p, vec3(.7));
    d = length(p) - 1.;
    return d;
}

float rayMarch(in vec3 ro, in vec3 rd) {
    float t = TMIN;
    for(int i = 0; i < RAYMARCH_TIME && t < TMAX; i++) {
        vec3 p = ro + t * rd;
        float d = map(p);
        if(d < PRECISION)
            break;
        t += d;
    }
    return t;
}

// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal(in vec3 p) {
    const float h = 0.0001;
    const vec2 k = vec2(1, -1);
    return normalize(k.xyy * map(p + k.xyy * h) +
        k.yyx * map(p + k.yyx * h) +
        k.yxy * map(p + k.yxy * h) +
        k.xxx * map(p + k.xxx * h));
}

mat3 setCamera(vec3 ta, vec3 ro, float cr) {
    vec3 z = normalize(ta - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.);
    vec3 x = normalize(cross(z, cp));
    vec3 y = cross(x, z);
    return mat3(x, y, z);
}

vec3 render(vec2 uv) {
    vec3 color = vec3(0.);
    vec3 ro = vec3(2. * cos(iTime), 1., 2. * sin(iTime));
    if (iMouse.z > 0.01) {
        float theta = iMouse.x / iResolution.x * 2. * PI;
        ro = vec3(2. * cos(theta), 2. * (-2. * iMouse.y / iResolution.y + 1.), 2. * sin(theta));
    }
    vec3 ta = vec3(0.);
    mat3 cam = setCamera(ta, ro, 0.);
    vec3 rd = normalize(cam * vec3(uv, 1.));
    float t = rayMarch(ro, rd);
    if(t < TMAX) {
        vec3 p = ro + t * rd;
        vec3 n = calcNormal(p);
        vec3 light = vec3(2., 1., 0.);
        float dif = clamp(dot(normalize(light - p), n), 0., 1.);
        float amb = 0.5 + 0.5 * dot(n, vec3(0., 1., 0.));

        // color = amb * vec3(.7) + dif * vec3(1.);
        vec3 colorXY = texture(iChannel0, p.xy * .5 + .5).rgb;
        vec3 colorXZ = texture(iChannel0, p.xz * .5 + .5).rgb;
        vec3 colorYZ = texture(iChannel0, p.yz * .5 + .5).rgb;
        n = abs(n);
        n = pow(n, vec3(10.));
        n /= n.x + n.y + n.z;
        color = colorXY * n.z + colorXZ * n.y + colorYZ * n.x;
        // color = n;
    }
    return color;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec3 color = vec3(0.);
    for(int m = 0; m < AA; m++) {
        for(int n = 0; n < AA; n++) {
            vec2 offset = 2. * (vec2(float(m), float(n)) / float(AA) - .5);
            vec2 uv = fixUV(fragCoord + offset);
            color += render(uv);
        }
    }
    fragColor = vec4(color / float(AA * AA), 1.);
}
