#pragma once

#include <memory>
#include <open3d/Open3D.h>
#include <string>

class PcdSampler {
public:
  PcdSampler(){};

  std::shared_ptr<open3d::geometry::PointCloud> toFPSPcd(
      std::shared_ptr<open3d::geometry::PointCloud> &pcd,
      const int &sample_point_num);

  const bool toFPSPcdFile(
      const std::string &pcd_file_path,
      const int &sample_point_num,
      const std::string &save_pcd_file_path);
};
