#pragma once

#include "pgr_params.h"
#include <torch/extension.h>

const torch::Tensor get_A(const torch::Tensor &x, const torch::Tensor &y,
                          const torch::Tensor &x_width);

const torch::Tensor mul_A_T(const torch::Tensor &x, const torch::Tensor &y,
                            const torch::Tensor &xi,
                            const torch::Tensor &x_width,
                            const int &chunk_size);

const torch::Tensor get_B(const torch::Tensor &x, const torch::Tensor &y,
                          const int &chunk_size, const torch::Tensor &x_width,
                          const float &alpha);

const torch::Tensor solveLSE(const torch::Tensor &x, const torch::Tensor &y,
                             const torch::Tensor &x_width,
                             const int &chunk_size, const float &iso_value,
                             const float &r_sq_stop_eps,
                             const PGRParams &pgr_params);
