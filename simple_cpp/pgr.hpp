#pragma once

#include "common.hpp"
#include <iostream>
#include <string>

void CloudMeshPGR(MeshData &ioMesh, const std::string &algo_folder_path,
                  const std::string &tmp_folder_path) {
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

  const int result_state =
      runPython(algo_folder_path + "param-gauss-recon/", command);

  if (result_state != 0) {
    std::cerr << "Error executing command: " << command << std::endl;
    return;
  }

  loadPlyFile(save_mesh_file_path, ioMesh);

  return;
}
