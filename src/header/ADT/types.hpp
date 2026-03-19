#pragma once

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