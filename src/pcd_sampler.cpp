#include "pcd_sampler.h"
#include "pcd.h"
#include <filesystem>

std::shared_ptr<open3d::geometry::PointCloud> PcdSampler::toFPSPcd(
    std::shared_ptr<open3d::geometry::PointCloud> &pcd,
    const int &sample_point_num){
  const int pcd_point_num = pcd->points_.size();
  if (pcd_point_num < sample_point_num){
    std::cout << "[WARN][PcdSampler::toFPSPcd]" << std::endl;
    std::cout << "\t pcd point num = " << pcd_point_num << " , < sample point num = " << sample_point_num << " !" << std::endl;
    std::cout << "\t will directly return source point cloud!" << std::endl;

    return pcd;
  }

  std::shared_ptr<open3d::geometry::PointCloud> down_sample_pcd = downSample(pcd, sample_point_num);

  return down_sample_pcd;
}

const bool PcdSampler::toFPSPcdFile(
    const std::string &pcd_file_path,
    const int &sample_point_num,
    const std::string &save_pcd_file_path){
  if (!std::filesystem::exists(pcd_file_path)){
    std::cout << "[ERROR][PcdSampler::toFPSPcdFile]" << std::endl;
    std::cout << "\t pcd file not exist!" << std::endl;
    std::cout << "\t pcd_file_path : " << pcd_file_path << std::endl;
    return false;
  }

  std::shared_ptr<open3d::geometry::PointCloud> pcd = std::make_shared<open3d::geometry::PointCloud>();
  if (!open3d::io::ReadPointCloud(pcd_file_path, *pcd)){
    std::cout << "[ERROR][PcdSampler::toFPSPcdFile]" << std::endl;
    std::cout << "\t ReadPointCloud failed!" << std::endl;
    std::cout << "\t pcd_file_path : " << pcd_file_path << std::endl;
    return false;
  }

  const std::string save_pcd_folder_path = std::filesystem::path(save_pcd_file_path).parent_path();

  std::shared_ptr<open3d::geometry::PointCloud> down_sample_pcd = toFPSPcd(pcd, sample_point_num);

  if (!std::filesystem::exists(save_pcd_folder_path)){
    std::filesystem::create_directories(save_pcd_folder_path);
  }

  if (!open3d::io::WritePointCloud(save_pcd_file_path, *down_sample_pcd, open3d::io::WritePointCloudOption("auto",
          open3d::io::WritePointCloudOption::IsAscii::Ascii))){
    std::cout << "[ERROR][PcdSampler::toFPSPcdFile]" << std::endl;
    std::cout << "\t WritePointCloud failed!" << std::endl;
    std::cout << "\t save_pcd_file_path : " << save_pcd_file_path << std::endl;
    return false;
  }

  return true;
}
