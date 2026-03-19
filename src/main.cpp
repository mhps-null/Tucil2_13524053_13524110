#include "ADT/types.hpp"
#include "geometry/Mesh.hpp"
#include "filesystem/FileSystem.hpp"
#include "geometry/Octree.hpp"

#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Cara penggunaan: " << argv[0] << " <file_name.obj> <max_depth>\n";
        std::cerr << "Contoh: " << argv[0] << " test.obj 4\n";
        return 1;
    }

    std::string fileName = argv[1];
    int maxDepth;

    try
    {
        maxDepth = std::stoi(argv[2]);
    }
    catch (...)
    {
        std::cerr << "[ERROR] <max_depth> harus berupa angka bulat!\n";
        return 1;
    }

    Mesh mesh;
    std::cout << "Membaca file " << fileName << "...\n";

    if (!FileSystem::loadObjFromFile(fileName, mesh))
    {
        std::cerr << "[ERROR] Gagal memuat file. Program berhenti.\n";
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    Octree octree(maxDepth);
    octree.buildTree(mesh);

    fileName = "voxelized_" + fileName;
    int voxelVerticesCount = 0;
    int voxelFacesCount = 0;

    FileSystem::saveObj(octree.root, maxDepth, fileName, voxelVerticesCount, voxelFacesCount);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_ms = end_time - start_time;

    std::cout << "\nOutput:\n";
    std::cout << "Banyak voxel yang terbentuk: " << octree.stats.numVoxels << "\n";
    std::cout << "Banyak vertex yang terbentuk: " << voxelVerticesCount << "\n";
    std::cout << "Banyak faces yang terbentuk: " << voxelFacesCount << "\n";

    std::cout << "\nStatistik node octree yang terbentuk:\n";
    for (int i = 1; i <= maxDepth; i++)
    {
        std::cout << i << " : " << octree.stats.nodesFormed[i] << "\n";
    }

    std::cout << "\nStatistik node yang tidak perlu ditelusuri:\n";
    for (int i = 1; i <= maxDepth; i++)
    {
        std::cout << i << " : " << octree.stats.nodesPruned[i] << "\n";
    }

    std::cout << "\nKedalaman octree: " << maxDepth << "\n";
    std::cout << "Lama waktu program berjalan: " << elapsed_ms.count() << " ms\n";
    std::cout << "Path dimana file .obj disimpan: test/output/" << fileName << "\n";

    return 0;
}