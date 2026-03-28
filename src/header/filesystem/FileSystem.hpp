#pragma once

#include <string>

#include "geometry/Octree.hpp"
#include "geometry/Mesh.hpp"
using namespace geometry;

class FileSystem
{
public:
    static bool loadObjFromFile(const std::string &fileName, Mesh &outMesh);
    static bool saveObj(OctreeNode *root, int maxDepth, const std::string &outputPath, int &outNumVertices, int &outNumFaces);

private:
    static void traverseAndGenerate(OctreeNode *node, int maxDepth, std::vector<Vertex> &outVertices, std::vector<Face> &outFaces);
};