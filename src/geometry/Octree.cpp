#include "geometry/Octree.hpp"

#include <algorithm>
#include <cmath>
#include <future> // asynchronus

OctreeNode::OctreeNode(const BoundingBox &b, int d) : box(b), depth(d), isLeaf(false)
{
    for (int i = 0; i < 8; i++)
    {
        children[i] = nullptr;
    }
}

OctreeNode::~OctreeNode()
{
    for (int i = 0; i < 8; i++)
    {
        if (children[i])
        {
            delete children[i];
            children[i] = nullptr;
        }
    }
};

void OctreeNode::build(const Mesh &mesh, int maxDepth, const std::vector<int> &surroundFaces, OctreeStats &stats)
{
    stats.addNodeFormed(this->depth);

    for (int faceIdx : surroundFaces)
    {
        if (isIntersectFace(mesh, faceIdx))
        {
            faceIndices.push_back(faceIdx);
        }
    }

    if (faceIndices.empty())
    {
        isLeaf = true;
        stats.addNodePruned(this->depth);
        return;
    }

    if (depth == maxDepth)
    {
        isLeaf = true;
        stats.addVoxel();
        return;
    }

    isLeaf = false;
    subdivide();

    if (this->depth < 2) // batasi concurrency maksimal depth 3, agar tidak overhead
    {
        std::vector<std::future<void>> futures;

        for (int i = 0; i < 8; i++)
        {
            futures.push_back(std::async(std::launch::async, [&, i]()
                                         { children[i]->build(mesh, maxDepth, this->faceIndices, stats); }));
        }

        for (auto &f : futures)
        {
            f.get();
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            children[i]->build(mesh, maxDepth, this->faceIndices, stats);
        }
    }

    faceIndices.clear();
}

void OctreeNode::subdivide()
{
    Vertex center;
    center.x = (box.min.x + box.max.x) / 2.0f;
    center.y = (box.min.y + box.max.y) / 2.0f;
    center.z = (box.min.z + box.max.z) / 2.0f;

    for (int i = 0; i < 8; i++)
    {
        BoundingBox childBox;

        if (i & 0b001) // kanan
        {
            childBox.min.x = center.x;
            childBox.max.x = box.max.x;
        }
        else // kiri
        {
            childBox.min.x = box.min.x;
            childBox.max.x = center.x;
        }

        if (i & 0b010) // atas
        {
            childBox.min.y = center.y;
            childBox.max.y = box.max.y;
        }
        else // bawah
        {
            childBox.min.y = box.min.y;
            childBox.max.y = center.y;
        }

        if (i & 0b100) // depan
        {
            childBox.min.z = center.z;
            childBox.max.z = box.max.z;
        }
        else // belakang
        {
            childBox.min.z = box.min.z;
            childBox.max.z = center.z;
        }
        children[i] = new OctreeNode(childBox, depth + 1);
    }
};

inline void findMinMax(float x1, float x2, float x3, float &min, float &max)
{
    min = std::min({x1, x2, x3});
    max = std::max({x1, x2, x3});
};

inline float dot(const float a[3], const float b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

bool OctreeNode::isIntersectFace(const Mesh &mesh, int faceIndex) const
{
    const Face &f = mesh.faces[faceIndex];
    const Vertex &v1 = mesh.vertices[f.v1];
    const Vertex &v2 = mesh.vertices[f.v2];
    const Vertex &v3 = mesh.vertices[f.v3];

    float c[3] = {
        (box.min.x + box.max.x) * 0.5f,
        (box.min.y + box.max.y) * 0.5f,
        (box.min.z + box.max.z) * 0.5f};
    float e[3] = {
        (box.max.x - box.min.x) * 0.5f,
        (box.max.y - box.min.y) * 0.5f,
        (box.max.z - box.min.z) * 0.5f};

    float v1Shifted[3] = {v1.x - c[0], v1.y - c[1], v1.z - c[2]};
    float v2Shifted[3] = {v2.x - c[0], v2.y - c[1], v2.z - c[2]};
    float v3Shifted[3] = {v3.x - c[0], v3.y - c[1], v3.z - c[2]};

    float f1[3] = {v2Shifted[0] - v1Shifted[0],
                   v2Shifted[1] - v1Shifted[1],
                   v2Shifted[2] - v1Shifted[2]};
    float f2[3] = {v3Shifted[0] - v2Shifted[0],
                   v3Shifted[1] - v2Shifted[1],
                   v3Shifted[2] - v2Shifted[2]};
    float f3[3] = {v1Shifted[0] - v3Shifted[0],
                   v1Shifted[1] - v3Shifted[1],
                   v1Shifted[2] - v3Shifted[2]};

    float triMin, triMax;
    findMinMax(v1Shifted[0], v2Shifted[0], v3Shifted[0], triMin, triMax);
    if (triMin > e[0] || triMax < -e[0])
        return false;
    findMinMax(v1Shifted[1], v2Shifted[1], v3Shifted[1], triMin, triMax);
    if (triMin > e[1] || triMax < -e[1])
        return false;
    findMinMax(v1Shifted[2], v2Shifted[2], v3Shifted[2], triMin, triMax);
    if (triMin > e[2] || triMax < -e[2])
        return false;

    // X × f1
    {
        float axis[3] = {0.0f, -f1[2], f1[1]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[1] * std::fabs(f1[2]) + e[2] * std::fabs(f1[1]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Y × f1
    {
        float axis[3] = {f1[2], 0.0f, -f1[0]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f1[2]) + e[2] * std::fabs(f1[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Z × f1
    {
        float axis[3] = {-f1[1], f1[0], 0.0f};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f1[1]) + e[1] * std::fabs(f1[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // X × f2
    {
        float axis[3] = {0.0f, -f2[2], f2[1]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[1] * std::fabs(f2[2]) + e[2] * std::fabs(f2[1]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Y × f2
    {
        float axis[3] = {f2[2], 0.0f, -f2[0]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f2[2]) + e[2] * std::fabs(f2[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Z × f2
    {
        float axis[3] = {-f2[1], f2[0], 0.0f};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f2[1]) + e[1] * std::fabs(f2[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // X × f3
    {
        float axis[3] = {0.0f, -f3[2], f3[1]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[1] * std::fabs(f3[2]) + e[2] * std::fabs(f3[1]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Y × f3
    {
        float axis[3] = {f3[2], 0.0f, -f3[0]};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f3[2]) + e[2] * std::fabs(f3[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    // Z × f3
    {
        float axis[3] = {-f3[1], f3[0], 0.0f};
        float p0 = dot(v1Shifted, axis);
        float p1 = dot(v2Shifted, axis);
        float p2 = dot(v3Shifted, axis);
        float r = e[0] * std::fabs(f3[1]) + e[1] * std::fabs(f3[0]);

        float minP = std::min({p0, p1, p2});
        float maxP = std::max({p0, p1, p2});
        if (minP > r || maxP < -r)
            return false;
    }

    float normal[3] = {
        f1[1] * f2[2] - f1[2] * f2[1],
        f1[2] * f2[0] - f1[0] * f2[2],
        f1[0] * f2[1] - f1[1] * f2[0]};

    float d = dot(normal, v1Shifted);

    float r_plane = e[0] * std::fabs(normal[0]) + e[1] * std::fabs(normal[1]) + e[2] * std::fabs(normal[2]);

    if (d > r_plane || d < -r_plane)
        return false;

    return true;
};

Octree::Octree(int depth) : root(nullptr), maxDepth(depth > 0 ? depth : 0), stats{depth > 0 ? depth : 0} {};

Octree::~Octree()
{
    if (root)
    {
        delete root;
        root = nullptr;
    }
};

void Octree::buildTree(const Mesh &mesh)
{
    if (root)
    {
        delete root;
        root = nullptr;
    }

    stats.numVoxels = 0;
    std::fill(stats.nodesFormed.begin(), stats.nodesFormed.end(), 0);
    std::fill(stats.nodesPruned.begin(), stats.nodesPruned.end(), 0);

    BoundingBox sceneBox = mesh.findBoundingBox();

    float eps = 0.001f;
    sceneBox.min.x -= eps;
    sceneBox.min.y -= eps;
    sceneBox.min.z -= eps;
    sceneBox.max.x += eps;
    sceneBox.max.y += eps;
    sceneBox.max.z += eps;

    root = new OctreeNode(sceneBox, 0);

    std::vector<int> allFaces;
    allFaces.reserve(mesh.faces.size());
    for (int i = 0; i < mesh.faces.size(); i++)
    {
        allFaces.push_back(i);
    }

    root->build(mesh, maxDepth, allFaces, this->stats);
};
