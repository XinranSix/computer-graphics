// Ray Marching
// #define TMIN 0.1
// #define TMAX 150.0
#define RAYMARCH_TIME 128
#define PRECISION 0.001
#define AA 3
#define PI 3.141592653

vec2 fixUV(in vec2 c) {
    return (2.0 * c - iResolution.xy) / min(iResolution.x, iResolution.y);
}

float sdfSphere(in vec3 p) {
    return length(p) - 1.0;
}

float sdfBox(in vec3 p, in vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

vec2 opU(vec2 a, vec2 b) {
    return a.x < b.x ? a : b;
}

vec2 map(in vec3 p) {
    vec2 d = vec2(sdfSphere(p - vec3(0.0, 1.0, 0.0)), 2.0);
    d = opU(d, vec2(sdfBox(p - vec3(2.0, 1.0, 0.0), vec3(0.5, 1.0, 1.0)), 3.0));
        
    return d;
}

vec2 rayMarch(in vec3 ro, in vec3 rd) {
    float t = 0.1;
    float tmax = 40.0;
    vec2 res = vec2(-1.0);
    if (rd.y < 0.0) {
        float tp = -ro.y / rd.y;
        tmax = min(tmax, tp);
        res = vec2(tp, 1.0);
    }
    for (int i = 0; i < RAYMARCH_TIME && t < tmax; ++i) {
        vec3 p = ro + t * rd;
        vec2 d = map(p);
        if (d.x < PRECISION) {
            res = vec2(t, d.y); 
            break;
        }
        t += d.x;
    }
    return res;
}

// 计算梯度
vec3 calcNormal(in vec3 p) {
    const float h = 0.0001;
    const vec2 k = vec2(1.0, -1.0);
    return normalize(
        k.xyy * map(p + k.xyy * h).x + 
        k.yyx * map(p + k.yyx * h).x + 
        k.yxy * map(p + k.yxy * h).x + 
        k.xxx * map(p + k.xxx * h).x
    );
}

mat3 setCamera(vec3 target, vec3 ro, float cr) {
    vec3 z = normalize(target - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.0);
    vec3 x = normalize(cross(z, cp));
    vec3 y = cross(x, z);
    return mat3(x, y, z); // 变换矩阵就是单位正交基的组合
}


float softShadow(in vec3 ro, in vec3 rd, float k) {
    float res = 1.0;
    float ph = 1e20;
    float tmin = 0.1;
    float tmax = 10.0;
    for (float t = tmin; t < tmax;) {
        float h = map(ro + rd * t).x;
        if (h < 0.001) {
            return 0.0;
        }
        float y = h * h / (2.0 * ph);
        float d = sqrt(h * h - y * y);
        res = min(res, k * d / max(0.0, t - y));
        ph = h;
        t += h;
    }
    return res;
}

vec2 tri(in vec2 x) {
    vec2 h = fract(x * 0.5) - 0.5;
    return 1.0 - 2.0 * abs(h);
}

float checkersGrad(in vec2 uv, in vec2 ddx, in vec2 ddy) {
    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;
    vec2 i = (tri(uv + 0.5 * w) - tri(uv - 0.5 * w)) / w;
    return 0.5 - 0.5 * i.x * i.y;
}

vec3 render(in vec2 uv, in vec2 px, in vec2 py) {
    vec3 ro = vec3(4.0 * cos(0.1 * iTime), 2, 4.0 * sin(0.1 * iTime));
    if (iMouse.z > 0.01) {
        float theta = iMouse.x / iResolution.x * 2.0 * PI;
        ro = vec3(4.0 * cos(theta), 2, 4.0 * sin(theta));
    }
    vec3 ta = vec3(0.0, 1.0, 0.0);
    mat3 cam = setCamera(ta, ro, 0.0);
    float fl = 1.0;
    vec3 rd = normalize(cam * vec3(uv, 1.0)); // camera space 中 camera 就是原点
    vec3 bg = vec3(0.7, 0.7, 0.9);
    vec3 color = bg - rd.y * vec3(0.1);
    vec2 t = rayMarch(ro, rd);
    if(t.y > 0.0) { 
        vec3 p = ro + t.x * rd; // 交点
        vec3 n = (t.y < 1.1) ? vec3(0.0, 1.0, 0.0) : calcNormal(p);
        vec3 light = vec3(5.0, 5.0, 0.0);
        float dif = clamp(dot(normalize(light - p), n), 0.0, 1.0);
        p += PRECISION * n;
        float st = softShadow(p, normalize(light - p), 10.0);
        dif *= st;
        float amb = 0.5 + 0.5 * dot(n, vec3(0.0, 1.0, 0.0));
        vec3 c = vec3(0.0);
        if (t.y > 1.9 && t.y < 2.1) {
            // sphere
            c = vec3(1.0, 0.0, 0.0);
        } else if (t.y > 2.9 && t.y < 3.1) {
            // box
            c = vec3(1.0, 1.0, 0.0);
        } else if (t.y > 0.9 && t.y < 1.1) {
            // plane
            vec3 rdx = normalize(cam * vec3(px, fl));
            vec3 rdy = normalize(cam * vec3(py, fl));
            vec3 ddx = ro.y * (rd / rd.y - rdx / rdx.y);
            vec3 ddy = ro.y * (rd / rd.y - rdy / rdy.y);
            c = vec3(0.3) + vec3(0.2) * checkersGrad(p.xz, ddx.xz, ddy.xz);
        }
        color = amb * c + dif * vec3(0.7);
    } 

    return sqrt(color); // 二次 gamma 校正
}

void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec3 color = vec3(0.0);
    for (int m = 0; m < AA; ++m) {
        for (int n = 0; n < AA; ++n) {
            vec2 offset = 2.0 * (vec2(float(m), float(n)) / float(AA) - 0.5);
            vec2 uv = fixUV(fragCoord + offset);
            vec2 px = fixUV(fragCoord + vec2(1.0, 0.0) + offset);
            vec2 py = fixUV(fragCoord + vec2(0.0, 1.0) + offset);
            color += render(uv, px, py);
        }
    }
    fragColor = vec4(color / float(AA * AA), 1.0);
}
