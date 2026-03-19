#include "Octree.hpp"

#include <iostream>

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
            delete children[i];
    }
};

void OctreeNode::build(const Mesh &mesh, int maxDepth, const std::vector<int> &sorroundFaces)
{
    for (int faceIdx : sorroundFaces)
    {
        if (isIntersectFace(mesh, faceIdx))
        {
            faceIndices.push_back(faceIdx);
        }
    }

    if (faceIndices.empty())
    {
        isLeaf = true;
        return;
    }

    if (depth == maxDepth)
    {
        isLeaf = true;
        return;
    }

    isLeaf = false;
    subdivide();

    for (int i = 0; i < 8; i++)
    {
        children[i]->build(mesh, maxDepth, this->faceIndices);
    }

    faceIndices.clear();
    faceIndices.shrink_to_fit();
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

bool OctreeNode::isIntersectFace(const Mesh &mesh, int faceIndex) const
{
    // belum
    return true;
};

Octree::Octree(int depth) : root(nullptr), maxDepth(depth) {};

Octree::~Octree()
{
    if (root)
        delete root;
};

void Octree::buildTree(const Mesh &mesh)
{
    if (root)
        delete root;

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

    root->build(mesh, maxDepth, allFaces);
};
