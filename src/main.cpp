#include "pgr_io.h"
#include "pgr_params.h"
#include "pgr_reconstructor.h"
#include <iostream>

void CloudMeshPGR(MeshData &ioMesh, const std::string &tmp_folder_path) {
  std::string valid_tmp_folder_path = tmp_folder_path;
  if (valid_tmp_folder_path.back() != '/') {
    valid_tmp_folder_path += '/';
  }

  const std::string save_pcd_file_path = valid_tmp_folder_path + "pcd.ply";
  const std::string save_normalize_pcd_file_path =
      valid_tmp_folder_path + "normalized_pcd.xyz";
  const std::string save_mesh_file_path =
      valid_tmp_folder_path + "pgr_recon.ply";
  const bool overwrite = true;

  savePcdFile(ioMesh.vertices, save_pcd_file_path);

  PGRParams pgr_params;

  PGRReconstructor().reconstructSurface(pgr_params, save_pcd_file_path,
                                        save_normalize_pcd_file_path,
                                        save_mesh_file_path, overwrite);

  MeshData mesh_data = readMeshData(save_mesh_file_path);

  ioMesh.vertices = mesh_data.vertices;
  ioMesh.indices = mesh_data.indices;
  ioMesh.normals = mesh_data.normals;
  ioMesh.cornerIndices = mesh_data.cornerIndices;

  return;
}

class DataClean {
public:
  DataClean() {};

  void CloudMesh(MeshData &ioMesh, const std::string &tmp_folder_path);
};

void DataClean::CloudMesh(MeshData &ioMesh,
                          const std::string &tmp_folder_path) {
  CloudMeshPGR(ioMesh, tmp_folder_path);
  return;
}

void testAPI(const std::string &pcd_file_path,
             const std::string &tmp_folder_path) {
  MeshData mesh_data = readPcdData(pcd_file_path);

  DataClean().CloudMesh(mesh_data, tmp_folder_path);

  std::cout << "mesh vertices size: " << mesh_data.vertices.size() / 3
            << std::endl;
  std::cout << "mesh indices size: " << mesh_data.indices.size() / 3
            << std::endl;
  std::cout << "mesh normals size: " << mesh_data.normals.size() / 3
            << std::endl;
  return;
}

int main() {
  PGRParams pgr_params;
  const std::string pcd_file_path = "/home/chli/chLi/Dataset/Famous/bunny.ply";
  const std::string save_normalize_pcd_file_path =
      "/home/chli/Downloads/bunny_normalized.xyz";
  const std::string save_mesh_file_path = "/home/chli/Downloads/bunny_pgr.ply";
  const bool overwrite = true;

  testAPI(pcd_file_path, "./output/test1/");
  return 1;

  PGRReconstructor pgr_reconstructor;
  pgr_reconstructor.reconstructSurface(pgr_params, pcd_file_path,
                                       save_normalize_pcd_file_path,
                                       save_mesh_file_path, overwrite);

  return 1;
}
