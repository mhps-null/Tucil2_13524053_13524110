#pragma once

/*
 * ADT untuk menyimpan data vertex, face, dan bounding box pada mesh.
 */

struct Vertex
{
    float x, y, z;
};

struct Face
{
    int v1, v2, v3;
};

struct BoundingBox
{
    Vertex min, max;
};

/*
 *  ADT untuk Object Viewer (Camera) sederhana.
 */

struct Camera {
   double x, y, z;
   double angleX, angleY, angleZ;
};

struct Coordinates
{
    float x, y;
};

struct ScreenData {
   int x_screenPositionOfPt, y_screenPositionOfPt;
   double zFar = 100;
   int width=640, height=480;
};