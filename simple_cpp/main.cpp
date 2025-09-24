#include "io.hpp"
#include <iostream>
#include <string>

enum class CloudMeshAlgo {
  Poisson, // Poisson Reconstruction
  PGR,     // Parametric Gauss Recon
  WNNC,    // Winding Number
};

void CloudMeshPGR(MeshData &ioMesh, const std::string &tmp_folder_path) {
  const int sample_point_num = 20000;
  const int wk = 7;
  const double wmax = 0.015;
  const double wmin = 0.0015;
  const double alpha = 1.05;
  const int d = 10;
  const int md = 1;
  const bool use_cpu = false;

  std::string valid_tmp_folder_path = tmp_folder_path;
  if (valid_tmp_folder_path.back() != '/') {
    valid_tmp_folder_path += '/';
  }

  const std::string save_pcd_file_path = valid_tmp_folder_path + "pcd.xyz";
  const std::string save_normalize_pcd_file_path =
      valid_tmp_folder_path + "normalized_pcd.xyz";
  const std::string save_mesh_file_path =
      valid_tmp_folder_path + "pgr_recon.ply";
  const bool overwrite = true;

  savePointCloudToXYZ(ioMesh.vertices, save_pcd_file_path, overwrite);

  std::string command = "python run_pgr.py";
  command += " " + save_pcd_file_path;
  command += " " + valid_tmp_folder_path;
  command += " " + save_mesh_file_path;
  command += " -s " + std::to_string(sample_point_num);
  command += " -wk " + std::to_string(wk);
  command += " -wmax " + std::to_string(wmax);
  command += " -wmin " + std::to_string(wmin);
  command += " -a " + std::to_string(alpha);
  command += " -d " + std::to_string(d);
  command += " -md " + std::to_string(md);

  if (use_cpu) {
    command += " --cpu";
  }

  const int result_state = system(command.c_str());

  if (result_state != 0) {
    std::cerr << "Error executing command: " << command << std::endl;
    return;
  }

  loadPlyFile(save_mesh_file_path, ioMesh);

  return;
}

namespace DataClean {
void CloudMesh(MeshData &ioMesh, const std::string &tmp_folder_path,
               const CloudMeshAlgo &algo = CloudMeshAlgo::Poisson) {
  switch (algo) {
  case CloudMeshAlgo::Poisson: {
    std::cout << "Poisson Reconstruction not implemented yet." << std::endl;
    break;
  }
  case CloudMeshAlgo::PGR: {
    CloudMeshPGR(ioMesh, tmp_folder_path);
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
             const std::string &tmp_folder_path,
             const CloudMeshAlgo &algo = CloudMeshAlgo::Poisson) {
  MeshData mesh_data;
  loadPlyFile(pcd_file_path, mesh_data);

  DataClean::CloudMesh(mesh_data, tmp_folder_path, algo);

  std::cout << "mesh vertices size: " << mesh_data.vertices.size() / 3
            << std::endl;
  std::cout << "mesh indices size: " << mesh_data.indices.size() / 3
            << std::endl;
  std::cout << "mesh normals size: " << mesh_data.normals.size() / 3
            << std::endl;
  return;
}

int main() {
  testAPI("/home/chli/chLi/Dataset/Famous/bunny-v2.ply", "./output/test1/",
          CloudMeshAlgo::PGR);
  return 1;
}
