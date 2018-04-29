//A simple program that uses ray-tracing to render a single triangle
//c++ -o raytri raytri.cpp -O3 -std=c++11 -DMOLLER_TRUMBORE

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <random>

#include "../include/geometry.h"

constexpr float kEpsilon = 1e-8;

inline float deg2rad(const float &deg)
{
    return deg * M_PI / 180;
}

inline float clamp(const float &lo, const float &hi, const float &v)
{
    return std::max(lo, std::min(hi, v));
}

//The main ray-triangle intersection routine. You can test both methoods:
//the geometric method and the Moller-Trumbore algorithm (use the flag -DMOLLER_TRUMBORE when you compile.

bool rayTriangleIntersect(
    const Vec3f &orig, const Vec3f &dir,
    const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
    float &t, float &u, float &v)
{
#ifdef MOLLER_TRUMBORE
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = dir.crossProduct(v0v2);
    float det = v0v1.dotProduct(pvec);
#ifdef CULLING
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon)
        return false;
#else
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon)
        return false;
#endif
    float invDet = 1 / det;

    Vec3f tvec = orig - v0;
    u = tvec.dotProduct(pvec) * invDet;
    if (u < 0 || u > 1)
        return false;

    Vec3f qvec = tvec.crossProduct(v0v1);
    v = dir.dotProduct(qvec) * invDet;
    if (v < 0 || u + v > 1)
        return false;

    t = v0v2.dotProduct(qvec) * invDet;

    return true;
#else
    // compute plane's normal
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    // no need to normalize
    Vec3f N = v0v1.crossProduct(v0v2); // N
    float denom = N.dotProduct(N);

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = N.dotProduct(dir);
    if (fabs(NdotRayDirection) < kEpsilon) // almost 0
        return false;                      // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    float d = N.dotProduct(v0);

    // compute t (equation 3)
    t = (N.dotProduct(orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0)
        return false; // the triangle is behind

    // compute the intersection point using equation 1
    Vec3f P = orig + t * dir;

    // Step 2: inside-outside test
    Vec3f C; // vector perpendicular to triangle's plane

    // edge 0
    Vec3f edge0 = v1 - v0;
    Vec3f vp0 = P - v0;
    C = edge0.crossProduct(vp0);
    if (N.dotProduct(C) < 0)
        return false; // P is on the right side

    // edge 1
    Vec3f edge1 = v2 - v1;
    Vec3f vp1 = P - v1;
    C = edge1.crossProduct(vp1);
    if ((u = N.dotProduct(C)) < 0)
        return false; // P is on the right side

    // edge 2
    Vec3f edge2 = v0 - v2;
    Vec3f vp2 = P - v2;
    C = edge2.crossProduct(vp2);
    if ((v = N.dotProduct(C)) < 0)
        return false; // P is on the right side;

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle
#endif
}

int main(int argc, char **argv)
{ 
    const uint32_t width = 640;
    const uint32_t height = 480;
    Vec3f cols[3] = {{0.6, 0.4, 0.1}, {0.1, 0.5, 0.3}, {0.1, 0.3, 0.7}};
    Vec3f *framebuffer = new Vec3f[width * height];
    Vec3f *pix = framebuffer;
    float fov = 51.52;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = width / (float)height;
    Vec3f orig(0);
    for (uint32_t j = 0; j < height; ++j)
    {
        for (uint32_t i = 0; i < width; ++i)
        {
            // compute primary ray
            float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)height) * scale;
            Vec3f dir(x, y, -1);
            dir.normalize();
            float t, u, v;
            if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v))
            {
                //Interpolate colors using the barycentric coordinates
                *pix = u * cols[0] + v * cols[1] + (1 - u - v) * cols[2];
                // uncomment this line if you want to visualize the row barycentric coordinates
                //*pix = Vec3f(u, v, 1 - u - v);
            }
            pix++;
        }
    }

    // Save result to a PPM image (keep these flags if you compile under Windows)
    std::ofstream ofs("./out.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n"
        << width << " " << height << "\n255\n";
    for (uint32_t i = 0; i < height * width; ++i)
    {
        char r = (char)(255 * clamp(0, 1, framebuffer[i].x));
        char g = (char)(255 * clamp(0, 1, framebuffer[i].y));
        char b = (char)(255 * clamp(0, 1, framebuffer[i].z));
        ofs << r << g << b;
    }

    ofs.close();

    delete[] framebuffer;

    return 0;
}
