#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "vec3.h"

class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(color c) : color_value(c) {}

    virtual color value(double u, double v, const point3& p) const override {
        return color_value;
    }

private:
    color color_value;
};

// noise texture 
class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    template <typename T> inline T lerp(const T &v0, const T &v1, float t) const {
        return v0 + (v1-v0)*std::max(0.f, std::min(1.f, t));
    }

    point3 rotate(const point3 &v) const {
        return point3(v.x()*0.00 + v.y()*0.80 + v.z()*0.60, v.x()*-0.80 + v.y()*0.36 + v.z()*-0.48, v.x()*-0.60 + v.y()*-0.48 + v.z()*0.64);
    }

    float hash(const float n) const {
        float x = sin(n)*43758.5453f;
        return x-floor(x);
    }

    float noise(const point3 &x) const {
        point3 p(floor(x.x()), floor(x.y()), floor(x.z()));
        point3 f(x.x()-p.x(), x.y()-p.y(), x.z()-p.z());
        f = f*(f*(point3(3.f, 3.f, 3.f)-f*2.f));
        //float n = p*point3(1.f, 57.f, 113.f);
        float n = p.x() * 1.f + p.y() * 57.f + p.z() * 113.f;
        return lerp(lerp(
                        lerp(hash(n +  0.f), hash(n +  1.f), f.x()),
                        lerp(hash(n + 57.f), hash(n + 58.f), f.x()), f.y()),
                   lerp(
                       lerp(hash(n + 113.f), hash(n + 114.f), f.x()),
                       lerp(hash(n + 170.f), hash(n + 171.f), f.x()), f.y()), f.z());
    }

    float fractal_brownian_motion(const point3 &x) const { // this is a bad noise function with lots of artifacts. TODO: find a better one
        point3 p = rotate(x);
        float f = 0;
        f += 0.5000*noise(p); p = p*2.32;
        f += 0.2500*noise(p); p = p*3.03;
        f += 0.1250*noise(p); p = p*2.61;
        f += 0.0625*noise(p);
        return f/0.9375;
    }


    virtual color value(double u, double v, const point3& p) const override {
        return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * fractal_brownian_motion(p)));
    }

private:
    double scale;
};

#endif
