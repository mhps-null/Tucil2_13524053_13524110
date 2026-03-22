#pragma once

#include "geometry/Mesh.hpp"

#include <vector>
#include <mutex> // agar tidak race condition

struct OctreeStats
{
    int numVoxels = 0;
    std::vector<int> nodesFormed;
    std::vector<int> nodesPruned;

    std::mutex statsMutex;

    OctreeStats(int maxDepth)
    {
        nodesFormed.resize(maxDepth + 1, 0);
        nodesPruned.resize(maxDepth + 1, 0);
    }

    void addVoxel()
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        numVoxels++;
    }

    void addNodePruned(int depth)
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        nodesPruned[depth]++;
    }

    void addNodeFormed(int depth)
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        nodesFormed[depth]++;
    }
};

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

    void build(const Mesh &mesh, int maxDepth, const std::vector<int> &surroundFaces, OctreeStats &stats);

private:
    void subdivide();
    bool isIntersectFace(const Mesh &mesh, int faceIndex) const;
};

class Octree
{
public:
    OctreeNode *root;
    int maxDepth;
    OctreeStats stats;

    Octree(int depth);
    ~Octree();

    void buildTree(const Mesh &mesh);
};