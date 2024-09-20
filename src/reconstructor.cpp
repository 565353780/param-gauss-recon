#include "reconstructor.h"
#include "constant.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <open3d/io/PointCloudIO.h>
#include <string>

const std::string Reconstructor::toSampledPcdFile(
    const std::string &input,
    const int &sample_point_num) {
  if (sample_point_num > 0){
    const std::string pcd_file_name = std::filesystem::path(input).filename().string();
    const std::string pcd_file_type = std::filesystem::path(input).extension().string();

    std::string save_pcd_file_name = pcd_file_name;

    size_t start_pos = save_pcd_file_name.find(pcd_file_type);
    if (start_pos != std::string::npos) {
        save_pcd_file_name.replace(start_pos, pcd_file_type.length(), "_sample-" + std::to_string(sample_point_num) + ".xyz");
    }
    const std::string save_pcd_file_path = "./output/sample_pcd/" + save_pcd_file_name;

    std::cout << "[INFO][Reconstructor::toSampledPcdFile]" << std::endl;
    std::cout << "\t start toFPSPcdFile..." << std::endl;
    if (!pcd_sampler_.toFPSPcdFile(input, sample_point_num, save_pcd_file_path)){
      std::cout << "[WARN][Reconstructor::toSampledPcdFile]" << std::endl;
      std::cout << "\t toFPSPcdFile failed!" << std::endl;
      std::cout << "\t try to start reconstruct with the input point cloud..." << std::endl;

      return input;
    }

    return save_pcd_file_path;
  }

  if (input.find(".xyz") != std::string::npos){
    return input;
  }

  const std::string pcd_file_name = std::filesystem::path(input).filename().string();
  const std::string pcd_file_type = std::filesystem::path(input).extension().string();

  std::string save_pcd_file_name = pcd_file_name;

  size_t start_pos = save_pcd_file_name.find(pcd_file_type);
  if (start_pos != std::string::npos) {
      save_pcd_file_name.replace(start_pos, pcd_file_type.length(), ".xyz");
  }
  const std::string save_pcd_file_path = "./output/sample_pcd/" + save_pcd_file_name;

  std::shared_ptr<open3d::geometry::PointCloud> pcd = std::make_shared<open3d::geometry::PointCloud>();
  if (!open3d::io::ReadPointCloud(input, *pcd)){
    std::cout << "[ERROR][Reconstructor::toSampledPcdFile]" << std::endl;
    std::cout << "\t ReadPointCloud failed!" << std::endl;
    std::cout << "\t input : " << input << std::endl;
    return "";
  }

  const std::string save_pcd_folder_path = std::filesystem::path(input).parent_path();
  if (!std::filesystem::exists(save_pcd_folder_path)){
    std::filesystem::create_directories(save_pcd_folder_path);
  }

  if (!open3d::io::WritePointCloud(save_pcd_file_path, *pcd, open3d::io::WritePointCloudOption("auto",
          open3d::io::WritePointCloudOption::IsAscii::Ascii))){
    std::cout << "[ERROR][Reconstructor::toSampledPcdFile]" << std::endl;
    std::cout << "\t WritePointCloud failed!" << std::endl;
    std::cout << "\t save_pcd_file_path : " << save_pcd_file_path << std::endl;
    return "";
  }

  return save_pcd_file_path;
}

const bool Reconstructor::reconstructSurface(
    PGRParams &pgr_params,
    const std::string &input,
    const std::string &save_mesh_file_path,
    const bool &overwrite){
  if (save_mesh_file_path != ""){
    if (std::filesystem::exists(save_mesh_file_path)){
      if (!overwrite){
        return true;
      }

      std::filesystem::remove(save_mesh_file_path);
    }
  }

  const std::string save_pcd_file_path = toSampledPcdFile(input, pgr_params.sample_point_num);
  if (save_pcd_file_path == ""){
    std::cout << "[ERROR][Reconstructor::reconstructSurface]" << std::endl;
    std::cout << "\t toSampledPcdFile failed!" << std::endl;
    std::cout << "\t input : " << input << std::endl;
    return false;
  }

  param_midfix = pgr_params.toLogStr();

  pgr_params.outputInfo();

  const std::string &in_filename = save_pcd_file_path;
  const std::string file_name = std::filesystem::path(in_filename).filename();
  const std::string data_index = file_name.substr(0, file_name.length() - 4);

  const std::string results_folder = "results/";
  if (std::filesystem::exists(results_folder)){
    std::filesystem::remove_all(results_folder);
  }

  const std::string sample_file_folder = results_folder + data_index + "/samples/";
  const std::string solve_file_folder = results_folder + data_index + "/solve/";
  const std::string recon_file_folder = results_folder + data_index + "/recon/";

  const std::string sample_file_prefix = sample_file_folder + data_index;
  const std::string solve_file_prefix = solve_file_folder + data_index;
  const std::string recon_file_prefix = recon_file_folder + data_index;

  if (!std::filesystem::exists(sample_file_folder)){
    std::filesystem::create_directories(sample_file_folder);
    std::filesystem::copy_file(in_filename, sample_file_folder + file_name);
  }

  if (!std::filesystem::exists(solve_file_folder)){
    std::filesystem::create_directories(solve_file_folder);
  }
  if (!std::filesystem::exists(recon_file_folder)){
    std::filesystem::create_directories(recon_file_folder);
  }

  const std::string build_octree_cmd = EXPORT_QUERY_EXE + " -i " + save_pcd_file_path + " -o " + sample_file_prefix + pgr_params.toCMDStr();
  std::cout << "[INFO][Reconstructor::reconstructSurface]" << std::endl;
  std::cout << "\t [EXECUTING] " << build_octree_cmd << std::endl;
  const int octree_state = system(build_octree_cmd.c_str());
  if (octree_state != 0){
    std::cout << "[ERROR][Reconstructor::reconstructSurface]" << std::endl;
    std::cout << "\t EXECUTING failed!" << std::endl;
    return false;
  }

  std::cout << "[INFO][Reconstructor::reconstructSurface]" << std::endl;
  std::cout << "\t start solve equation..." << std::endl;
  solver_.solve(
      sample_file_prefix + "_normalized.npy",
      sample_file_prefix + "_for_query.npy",
      solve_file_prefix + param_midfix,
      pgr_params);

  if (!pgr_params.recon_mesh){
    return true;
  }

  const std::string isoval_txt_file_path = solve_file_prefix + param_midfix + "_isoval.txt";

  std::ifstream infile(isoval_txt_file_path);

  if (!infile) {
    std::cout << "[ERROR][Reconstructor::reconstructSurface]" << std::endl;
    std::cout << "\t open isoval file failed!" << std::endl;
    std::cout << "\t isoval_txt_file_path : " << isoval_txt_file_path << std::endl;
    return false;
  }

  std::string isoval;
  
  std::getline(infile, isoval);
  infile.close();

  const std::string recon_cmd = LOAD_QUERY_EXE
    + " -i " + in_filename  + pgr_params.toCMDStr()
    + " --grid_val " + solve_file_prefix + param_midfix + "_eval_grid.npy"
    + " --grid_width " + solve_file_prefix + param_midfix + "_grid_width.npy"
    + " --isov " + isoval
    + " -o " + recon_file_prefix + param_midfix + "_recon.ply";
  std::cout << "[INFO][Reconstructor::reconstructSurface]" << std::endl;
  std::cout << "\t [EXECUTING] " << recon_cmd << std::endl;
  const int recon_state = system(recon_cmd.c_str());

  if (recon_state != 0){
    std::cout << "[ERROR][Reconstructor::reconstructSurface]" << std::endl;
    std::cout << "\t EXECUTING failed!" << std::endl;
    return false;
  }

  if (save_mesh_file_path == ""){
    const std::string recon_file_basename = std::filesystem::path(recon_file_prefix).filename();
    const std::string save_recon_folder_path = "./output/recon/" + param_midfix.substr(1, param_midfix.length()) + "/";
    if (std::filesystem::exists(save_recon_folder_path)){
      std::filesystem::remove_all(save_recon_folder_path);
    }

    if (!std::filesystem::exists(save_recon_folder_path)){
      std::filesystem::create_directories(save_recon_folder_path);
    }

    const std::string copied_file_path = save_recon_folder_path + recon_file_basename + "_recon_pgr.ply";
    if (std::filesystem::exists(copied_file_path)){
      std::filesystem::remove(copied_file_path);
    }

    std::filesystem::copy_file("./" + recon_file_prefix + param_midfix + "_recon.ply",
        copied_file_path);

    return true;
  }

  const std::string save_mesh_folder_path = std::filesystem::path(save_mesh_file_path).parent_path();
  if (!std::filesystem::exists(save_mesh_folder_path)){
    std::filesystem::create_directories(save_mesh_folder_path);
  }

  std::filesystem::copy_file("./" + recon_file_prefix + param_midfix + "_recon.ply", save_mesh_file_path);

  return true;
}
