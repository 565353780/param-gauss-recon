#pragma once

#include "pgr_params.h"
#include <torch/extension.h>
#include <open3d/Open3D.h>

const torch::Tensor get_query_vals(
    const torch::Tensor &queries,
    const torch::Tensor &q_width,
    const torch::Tensor &y_base,
    const torch::Tensor &lse,
    const int &chunk_size);

const torch::Tensor get_width(
    const torch::Tensor &query_set,
    const PGRParams &pgr_params,
    const open3d::geometry::KDTreeFlann &base_kdtree);
