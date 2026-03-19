#pragma once

#include "Mesh.hpp"

#include <vector>

class OctreeNode
{
public:
    BoundingBox box;
    OctreeNode *children[8];
    std::vector<int> faceIndices;
    int depth;
    bool isLeaf = false;

    OctreeNode(const BoundingBox &b, int d);
    ~OctreeNode();

    void build(const Mesh &mesh, int maxDepth, const std::vector<int> &sorroundFaces);

private:
    void subdivide();
    bool isIntersectFace(const Mesh &mesh, int faceIndex) const;
};

class Octree
{
public:
    OctreeNode *root;
    int maxDepth;

    Octree(int depth);
    ~Octree();

    void buildTree(const Mesh &mesh);
};