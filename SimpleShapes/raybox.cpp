//A simple program to compute the intersection of rays with AA boxes

// c++ -o raybox raybox.cpp -O3 -std=c++11

#include "../include/geometry.h"
#include <cstdlib>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

class Ray
{
  public:
    Ray(const Vec3f &orig, const Vec3f &dir) : orig(orig), dir(dir)
    {
        invdir = 1 / dir;
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }
    Vec3f orig, dir; // ray orig and dir
    Vec3f invdir;
    int sign[3];
};

class AABBox
{
  public:
    AABBox(const Vec3f &b0, const Vec3f &b1) { bounds[0] = b0, bounds[1] = b1; }
    bool intersect(const Ray &r, float &t) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
        tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
        tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
        tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
        tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        t = tmin;

        if (t < 0)
        {
            t = tmax;
            if (t < 0)
                return false;
        }

        return true;
    }
    Vec3f bounds[2];
};

int main(int argc, char **argv)
{
    AABBox box(Vec3f(-1), Vec3f(1));
    gen.seed(0);
    for (uint32_t i = 0; i < 16; ++i)
    {
        Vec3f randDir(2 * dis(gen) - 1, 2 * dis(gen) - 1, 2 * dis(gen) - 1);
        randDir.normalize();
        Ray ray(Vec3f(0), randDir);
        float t;
        if (box.intersect(ray, t))
        {
            Vec3f Phit = ray.orig + ray.dir * t;
            std::cerr << ray.orig << " " << Phit << std::endl;
        }
    }
    return 0;
}
