#include "FileSystem.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

bool FileSystem::loadObjFromFile(const std::string &fileName, Mesh &outMesh)
{
    std::filesystem::path filePath = std::filesystem::path("test/input") / fileName;

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
                std::cerr << "[ERROR] Format tidak valid! Ditemukan poligon non-segitiga (quadrilateral dsb)\n";
                std::cerr << "[ERROR] Error di baris " << lineNumber << "\n";
                return false;
            }

            Face f;
            f.v1 = std::stoi(s1) - 1;
            f.v2 = std::stoi(s2) - 1;
            f.v3 = std::stoi(s3) - 1;

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