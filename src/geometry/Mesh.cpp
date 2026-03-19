#include "Mesh.hpp"
#include <limits>
#include <algorithm>

BoundingBox Mesh::findBoundingBox() const
{
    BoundingBox box;

    float inf = std::numeric_limits<float>::infinity();
    box.min = {inf, inf, inf};
    box.max = {-inf, -inf, -inf};

    if (vertices.empty())
    {
        return box;
    }

    for (const auto &v : vertices)
    {
        box.min.x = std::min(box.min.x, v.x);
        box.min.y = std::min(box.min.y, v.y);
        box.min.z = std::min(box.min.z, v.z);

        box.max.x = std::max(box.max.x, v.x);
        box.max.y = std::max(box.max.y, v.y);
        box.max.z = std::max(box.max.z, v.z);
    }

    return box;
}