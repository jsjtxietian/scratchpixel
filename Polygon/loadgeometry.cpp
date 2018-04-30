//A simple program to load geometry data from a file
//c++ -o loadgeometry loadgeometry.cpp -O3 -std=c++11

#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <memory>
#include "../include/geometry.h"

void createMesh(
    const uint32_t nfaces,
    std::unique_ptr<uint32_t[]> &faceIndex,
    const uint32_t &vertsIndexArraySize,
    std::unique_ptr<uint32_t[]> &vertsIndex,
    const uint32_t &versArraySize,
    std::unique_ptr<Vec3f[]> &verts,
    std::unique_ptr<Vec3f[]> &normals,
    std::unique_ptr<Vec2f[]> &st)
{
}

// Structure of the geo file (ascii):
//     * nfaces (integer)
//     * face index array (array of integers)
//     * vertex index array (array of integers)
//     * vertex array (array of Vec3f, coordinates are seperated by a space)
//     * normal araay (array of Vec3f, coordinates are seperated by a space)
//     * texture coordinates array (array of Vec2f, coordinates are seperated by a space)

void loadGeoFile(const char *file)
{
    std::ifstream ifs;
    try
    {
        ifs.open(file);
        if (ifs.fail())
            throw;
        std::stringstream ss;
        ss << ifs.rdbuf();
        uint32_t numFaces;
        ss >> numFaces;
        std::cerr << "Mesh has " << numFaces << " faces " << std::endl;
        std::unique_ptr<uint32_t[]> faceIndex(new uint32_t[numFaces]);
        uint32_t vertsIndexArraySize = 0;
        // reading face index array
        for (uint32_t i = 0; i < numFaces; ++i)
        {
            ss >> faceIndex[i];
            vertsIndexArraySize += faceIndex[i];
            std::cerr << faceIndex[i] << std::endl;
        }
        std::cerr << "Verts index array size " << vertsIndexArraySize << std::endl;
        std::unique_ptr<uint32_t[]> vertsIndex(new uint32_t[vertsIndexArraySize]);
        uint32_t vertsArraySize = 0;
        // reading vertex index array
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
        {
            ss >> vertsIndex[i];
            if (vertsIndex[i] > vertsArraySize)
                vertsArraySize = vertsIndex[i];
            std::cerr << vertsIndex[i] << std::endl;
        }
        vertsArraySize += 1;
        std::cerr << "Max verts index " << vertsArraySize << std::endl;
        // reading vertices
        std::unique_ptr<Vec3f[]> verts(new Vec3f[vertsArraySize]);
        for (uint32_t i = 0; i < vertsArraySize; ++i)
        {
            ss >> verts[i].x >> verts[i].y >> verts[i].z;
            std::cerr << verts[i] << std::endl;
        }
        // reading normals
        std::cerr << "Reading normals\n";
        std::unique_ptr<Vec3f[]> normals(new Vec3f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
        {
            ss >> normals[i].x >> normals[i].y >> normals[i].z;
            std::cerr << normals[i] << std::endl;
        }
        // reading st coordinates
        std::cerr << "Reading texture coordinates\n";
        std::unique_ptr<Vec2f[]> st(new Vec2f[vertsIndexArraySize]);
        for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
        {
            ss >> st[i].x >> st[i].y;
            std::cerr << st[i] << std::endl;
        }

        createMesh(numFaces, faceIndex, vertsIndexArraySize, vertsIndex, vertsArraySize, verts, normals, st);
    }
    catch (...)
    {
        ifs.close();
    }
    ifs.close();
}

int main(int argc, char **argv)
{
    loadGeoFile("./test.geo");

    return 0;
}
