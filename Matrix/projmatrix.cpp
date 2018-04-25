//c++ -o projmatrix projmatrix.cpp -std=c++11 -O3

#include "../include/geometry.h"
#include "vertexdata.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>

#define M_PI 3.1416927657

//Set the basic perspective projection matrix
void setProjectionMatrix(const float &angleOfView,
                         const float &near,
                         const float &far,
                         Matrix44f &M)
{
    float scale = 1 / tan(angleOfView * 0.5 * M_PI / 180);
    M[0][0] = scale;
    M[1][1] = scale;
    M[2][2] = -far / (far - near);
    M[3][2] = -far * near / (far - near);
    M[2][3] = -1;
    M[3][3] = 0;
}

// Point-Matrix multiplication. The input point is assumed to have Cartesian
// coordinates but because we will multiply this point by a 4x4 matrix we actually
// assume it is a point with homogeneous coordinatess (x, y, z, w = 1). The point-matrix
// results in another point with homogeneous coordinates (x', y', z', w'). To get back to
// Cartesian coordinates we need to noramlized these coordinates: (x'/w', y'/w', z'/w').
void multPointMatrix(const Vec3f &in, Vec3f &out, const Matrix44f &M)
{
    //out = in * Mproj
    out.x = in.x * M[0][0] + in.y * M[1][0] + in.z * M[2][0] + /* in.z = 1 */ M[3][0];
    out.y = in.x * M[0][1] + in.y * M[1][1] + in.z * M[2][1] + /* in.z = 1 */ M[3][1];
    out.z = in.x * M[0][2] + in.y * M[1][2] + in.z * M[2][2] + /* in.z = 1 */ M[3][2];
    float w = in.x * M[0][3] + in.y * M[1][3] + in.z * M[2][3] + /* in.z = 1 */ M[3][3];

    if (w != 1)
    {
        out.x /= w;
        out.y /= w;
        out.z /= w;
    }
}

int main()
{
    uint32_t imageWidth = 512, imageHeight = 512;
    Matrix44f Mproj;
    Matrix44f worldToCamera;

    worldToCamera[3][1] = -10;
    worldToCamera[3][2] = -20;
    float angleOfView = 90;
    float near = 0.1;
    float far = 100;

    //Set the basic perspective projection matrix
    setProjectionMatrix(angleOfView, near, far, Mproj);
    unsigned char *buffer = new unsigned char[imageHeight * imageWidth];
    memset(buffer, 0, imageHeight * imageWidth);

    //loop over all points
    for (uint32_t i = 0; i < numVertices; i++)
    {
        Vec3f vertCamera, projectedVert;
        //transform to camera space
        multPointMatrix(vertices[i], vertCamera, worldToCamera);
        //project
        multPointMatrix(vertCamera, projectedVert, Mproj);
        if (projectedVert.x < -1 || projectedVert.x > 1 || projectedVert.y < -1 || projectedVert.y > 1)
            continue;
        uint32_t x = std::min(imageWidth - 1, (uint32_t)((projectedVert.x + 1) * 0.5 * imageWidth));
        uint32_t y = std::min(imageHeight - 1, (uint32_t)((1 - (projectedVert.y + 1) * 0.5) * imageHeight));
        buffer[y * imageWidth + x] = 255;
    }
    // export to image
    std::ofstream ofs;
    ofs.open("./out.ppm");
    ofs << "P5\n"
        << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write((char *)buffer, imageWidth * imageHeight);
    ofs.close();
    delete[] buffer;

    return 0;
}
