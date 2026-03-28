#pragma once

#include "geometry/Mesh.hpp"
#include "ADT/types.hpp"
#include <cmath>

using namespace std;

/*
 *  ADT untuk Object Viewer (Camera) sederhana.
 */

 // Posisi kamera
struct Camera {
    Vertex target{0.0f, 0.0f, 0.0f};
    float distance{5.0f}; 
    float yaw{0.0f}; // rotasi horizontal
    float pitch{0.0f}; // rotasi vertikal
    float fov{90.0f}; // field of view
    float nearPlane{0.1f};
    float farPlane{100.0f};
};

// Titik di layar per pixel
struct ScreenPoint {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    bool visible{false};
};

// Ukuran window
struct Viewport
{
    int width{640};
    int height{480};
};


// BBrp hal yang bakal dipakai untuk logic camera
namespace CameraLogic
{
    Vertex sub(const Vertex& a, const Vertex& b);

    float dot(const Vertex& a, const Vertex& b);
    Vertex cross(const Vertex& a, const Vertex& b);
    float length(const Vertex& v);
    Vertex normalize(const Vertex& v);

    Vertex getMeshCenter(const Mesh& mesh);
    float getMeshRadius(const Mesh& mesh);

    Vertex getCameraPosition(const Camera& cam);
    Vertex worldToCamera(const Vertex& point, const Camera& cam);

    ScreenPoint projectPoint(const Vertex& point, const Camera& cam, const Viewport& viewport);
}