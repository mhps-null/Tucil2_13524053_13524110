#include "types.hpp"
#include "Mesh.hpp"
#include "FileSystem.hpp"
#include "Octree.hpp"

#include <iostream>
#include <filesystem>
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Cara penggunaan: " << argv[0] << " <file_name.obj> <max_depth>\n";
        std::cerr << "Contoh: " << argv[0] << " test.obj 4\n";
        return 1;
    }
}