#include "pgr_pcd.h"
#include <cmath>

std::shared_ptr<open3d::geometry::PointCloud>
downSample(std::shared_ptr<open3d::geometry::PointCloud> &pcd,
           const int &sample_point_num, const float &voxel_size) {
  if (sample_point_num < 1) {
    std::cout << "[WARN][pgr_pcd::downSample]" << std::endl;
    std::cout << "\t sample point num < 1! will use source pcd!" << std::endl;
    std::cout << "\t sample_point_num : " << sample_point_num << std::endl;
    return pcd;
  }

  if (voxel_size > 0) {
    std::shared_ptr<open3d::geometry::PointCloud> down_sample_pcd =
        pcd->VoxelDownSample(voxel_size);
    return down_sample_pcd;
  }

  try {
    std::shared_ptr<open3d::geometry::PointCloud> down_sample_pcd =
        pcd->FarthestPointDownSample(sample_point_num);
    return down_sample_pcd;
  } catch (const std::exception &e) {
    const int every_k_points =
        std::ceil(pcd->points_.size() / sample_point_num);
    std::shared_ptr<open3d::geometry::PointCloud> down_sample_pcd =
        pcd->UniformDownSample(sample_point_num);
    return down_sample_pcd;
  }
}
