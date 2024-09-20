#pragma once

#include <torch/extension.h>

struct PointCloud {
  torch::Tensor points;

  PointCloud(torch::Tensor pts) {
    points = pts.clone().cpu().toType(torch::kFloat32);
    assert(points.dim() == 2);
  }

  inline size_t kdtree_get_point_count() const { return points.size(0); }

  inline float kdtree_get_pt(const size_t idx, const size_t dim) const {
    return points[idx][dim].item<float>();
  }

  template <class BBOX> bool kdtree_get_bbox(BBOX &) const { return false; }
};
