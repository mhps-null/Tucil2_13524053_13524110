#pragma once

#include "Mesh.hpp"

#include <string>

class FileSystem
{
public:
    static bool loadObjFromFile(const std::string &fileName, Mesh &outMesh);
};