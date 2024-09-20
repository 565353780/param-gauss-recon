#pragma once

#include <open3d/Open3D.h>

std::shared_ptr<open3d::geometry::PointCloud>
downSample(std::shared_ptr<open3d::geometry::PointCloud> &pcd,
           const int &sample_point_num, const float &voxel_size = -1.0);
