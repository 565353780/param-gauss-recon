#include "pgr.hpp"
#include <iostream>
#include <string>

enum class CloudMeshAlgo {
  Poisson, // Poisson Reconstruction
  PGR,     // Parametric Gauss Recon
  WNNC,    // Winding Number
};

namespace DataClean {
void CloudMesh(MeshData &ioMesh, const std::string &algo_folder_path,
               const std::string &tmp_folder_path,
               const CloudMeshAlgo &algo = CloudMeshAlgo::Poisson) {
  switch (algo) {
  case CloudMeshAlgo::Poisson: {
    std::cout << "Poisson Reconstruction not implemented yet." << std::endl;
    break;
  }
  case CloudMeshAlgo::PGR: {
    CloudMeshPGR(ioMesh, algo_folder_path, tmp_folder_path);
    break;
  }
  case CloudMeshAlgo::WNNC: {
    std::cout << "WNNC not implemented yet." << std::endl;
    break;
  }
  }

  return;
}
} // namespace DataClean

void testAPI(const std::string &pcd_file_path,
             const std::string &algo_folder_path,
             const std::string &tmp_folder_path,
             const CloudMeshAlgo &algo = CloudMeshAlgo::Poisson) {
  MeshData mesh_data;
  const bool load_success = loadPlyFile(pcd_file_path, mesh_data);
  if (!load_success) {
    std::cerr << "Failed to load point cloud from: " << pcd_file_path
              << std::endl;
    return;
  }

  DataClean::CloudMesh(mesh_data, algo_folder_path, tmp_folder_path, algo);

  std::cout << "mesh vertices size: " << mesh_data.vertices.size() / 3
            << std::endl;
  std::cout << "mesh indices size: " << mesh_data.indices.size() / 3
            << std::endl;
  std::cout << "mesh normals size: " << mesh_data.normals.size() / 3
            << std::endl;
  return;
}

int main() {
  std::vector<std::string> pcd_file_path_vec;
  pcd_file_path_vec.push_back("/home/chli/chLi/Dataset/Famous/bunny-v2.ply");
  pcd_file_path_vec.push_back("C:/github/bunny-v2.ply");

  std::vector<std::string> algo_folder_path_vec;
  algo_folder_path_vec.push_back(
      "/home/chli/github/AMCAX/next-shape/SurfProcess/");
  algo_folder_path_vec.push_back("C:/github/AMCAX/next-shape/SurfProcess/");

  for (auto pcd_file_path : pcd_file_path_vec) {
    if (std::filesystem::exists(pcd_file_path)) {
      for (auto algo_folder_path : algo_folder_path_vec) {
        if (std::filesystem::exists(algo_folder_path)) {
          testAPI(pcd_file_path, algo_folder_path,
                  algo_folder_path + "output/test_PGR/", CloudMeshAlgo::PGR);
        }
      }
    }
  }
  return 1;
}
