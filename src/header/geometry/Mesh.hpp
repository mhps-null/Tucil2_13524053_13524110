#pragma once

#include "types.hpp"

#include <vector>

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    BoundingBox findBoundingBox() const;
};