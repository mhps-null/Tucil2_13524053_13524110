#pragma once

#include "geometry/Mesh.hpp"
#include "filesystem/FileSystem.hpp"
#include "objectviewer/CameraLogic.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>

using namespace std;
using namespace sf;

// Penggunaan SFML
class Viewer{
    public:
        static void show(const Mesh& mesh);
};