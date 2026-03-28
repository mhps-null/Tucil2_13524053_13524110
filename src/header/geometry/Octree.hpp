#pragma once

#include "geometry/Mesh.hpp"

#include <atomic>
#include <vector>

using namespace geometry;

class ThreadPool;

struct OctreeStats
{
    std::atomic<int> numVoxels = 0;
    std::vector<std::atomic<int>> nodesFormed;
    std::vector<std::atomic<int>> nodesPruned;

     OctreeStats(int maxDepth): nodesFormed(maxDepth + 1), nodesPruned(maxDepth + 1)
    {
        for (int i = 0; i <= maxDepth; i++) {
            nodesFormed[i].store(0, std::memory_order_relaxed);
            nodesPruned[i].store(0, std::memory_order_relaxed);
        }
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

    void build(const Mesh &mesh, int maxDepth, const std::vector<int> &surroundFaces, OctreeStats &stats, ThreadPool *threadPool);

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