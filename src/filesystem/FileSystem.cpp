#include "filesystem/FileSystem.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

bool FileSystem::loadObjFromFile(const std::string &fileName, Mesh &outMesh)
{
    std::filesystem::path filePath =
        std::filesystem::weakly_canonical(
            std::filesystem::current_path() / "../test/input" / fileName);

    if (!(std::filesystem::exists(filePath)))
    {
        std::cerr << "[ERROR] File tidak ada!\n";
        return false;
    }

    if (!(std::filesystem::is_regular_file(filePath)))
    {
        std::cerr << "[ERROR] Path bukan file!\n";
        return false;
    }

    if (filePath.extension() != ".obj")
    {
        std::cerr << "[ERROR] Format file bukan .obj!\n";
        return false;
    }

    std::ifstream file(filePath);

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        lineNumber++;
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v")
        {
            Vertex v;
            if (!(ss >> v.x >> v.y >> v.z))
            {
                std::cerr << "[ERROR] Format vertex tidak valid di baris " << lineNumber << "\n";
                return false;
            }

            std::string extra;
            if (ss >> extra)
            {
                std::cerr << "[ERROR] Terlalu banyak argumen untuk vertex di baris " << lineNumber << "\n";
                return false;
            }

            outMesh.vertices.push_back(v);
        }
        else if (type == "f")
        {
            std::string s1, s2, s3;
            if (!(ss >> s1 >> s2 >> s3))
            {
                std::cerr << "[ERROR] Face butuh 3 vertex di baris " << lineNumber << "\n";
                return false;
            }

            std::string extra;
            if (ss >> extra)
            {
                std::cerr << "[ERROR] Face bukan segitiga (quadrilateral dsb) di baris " << lineNumber << "\n";
                return false;
            }

            Face f;
            try
            {
                f.v1 = std::stoi(s1) - 1;
                f.v2 = std::stoi(s2) - 1;
                f.v3 = std::stoi(s3) - 1;
            }
            catch (...)
            {
                std::cerr << "[ERROR] Format index face tidak valid di baris " << lineNumber << ": " << line << "\n";
                return false;
            }

            if (f.v1 < 0 || f.v1 >= outMesh.vertices.size() ||
                f.v2 < 0 || f.v2 >= outMesh.vertices.size() ||
                f.v3 < 0 || f.v3 >= outMesh.vertices.size())
            {
                std::cerr << "[ERROR] Index face out of bounds di baris " << lineNumber << "\n";
                return false;
            }

            outMesh.faces.push_back(f);
        }
    }

    file.close();
    return true;
}

bool FileSystem::saveObj(OctreeNode *root, int maxDepth, const std::string &fileName, int &outNumVertices, int &outNumFaces)
{
    std::vector<Vertex> voxelVertices;
    std::vector<Face> voxelFaces;

    traverseAndGenerate(root, maxDepth, voxelVertices, voxelFaces);

    outNumVertices = voxelVertices.size();
    outNumFaces = voxelFaces.size();

    std::filesystem::path filePath =
        std::filesystem::weakly_canonical(
            std::filesystem::current_path() / "../test/output" / fileName);

    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        std::cerr << "[ERROR] Gagal membuat file output: " << filePath << "\n";
        return false;
    }

    for (const auto &v : voxelVertices)
    {
        outFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    for (const auto &f : voxelFaces)
    {
        outFile << "f " << (f.v1 + 1) << " " << (f.v2 + 1) << " " << (f.v3 + 1) << "\n";
    }

    outFile.close();
    return true;
};

void FileSystem::traverseAndGenerate(OctreeNode *node, int maxDepth, std::vector<Vertex> &outVertices, std::vector<Face> &outFaces)
{
    if (!node)
        return;

    if (node->depth == maxDepth && !node->faceIndices.empty())
    {
        int vOffset = outVertices.size();

        float x1 = node->box.min.x, x2 = node->box.max.x;
        float y1 = node->box.min.y, y2 = node->box.max.y;
        float z1 = node->box.min.z, z2 = node->box.max.z;

        outVertices.push_back({x1, y1, z1}); // 0
        outVertices.push_back({x2, y1, z1}); // 1
        outVertices.push_back({x2, y2, z1}); // 2
        outVertices.push_back({x1, y2, z1}); // 3
        outVertices.push_back({x1, y1, z2}); // 4
        outVertices.push_back({x2, y1, z2}); // 5
        outVertices.push_back({x2, y2, z2}); // 6
        outVertices.push_back({x1, y2, z2}); // 7

        int indices[36] = {
            0, 2, 1, 0, 3, 2, // Depan
            1, 6, 5, 1, 2, 6, // Kanan
            5, 7, 4, 5, 6, 7, // Belakang
            4, 3, 0, 4, 7, 3, // Kiri
            3, 6, 2, 3, 7, 6, // Atas
            4, 1, 5, 4, 0, 1  // Bawah
        };

        for (int i = 0; i < 36; i += 3)
        {
            Face f;
            f.v1 = vOffset + indices[i];
            f.v2 = vOffset + indices[i + 1];
            f.v3 = vOffset + indices[i + 2];
            outFaces.push_back(f);
        }
        return;
    }

    if (!node->isLeaf)
    {
        for (int i = 0; i < 8; i++)
        {
            traverseAndGenerate(node->children[i], maxDepth, outVertices, outFaces);
        }
    }
}