#pragma once

#include "ADT/types.hpp"

#include <vector>

using namespace geometry;

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    BoundingBox findBoundingBox() const;
};