#pragma once

#include "pgr_params.h"
#include "pointcloud.hpp"
#include <nanoflann.hpp>
#include <torch/extension.h>

using namespace nanoflann;

typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PointCloud>,
                                 PointCloud, 3>
    KDTree;

const torch::Tensor get_query_vals(const torch::Tensor &queries,
                                   const torch::Tensor &q_width,
                                   const torch::Tensor &y_base,
                                   const torch::Tensor &lse,
                                   const int &chunk_size);

const torch::Tensor get_width(const torch::Tensor &query_set,
                              const PGRParams &pgr_params, KDTree &base_kdtree);
