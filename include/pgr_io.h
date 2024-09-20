#pragma once

#include <cnpy.h>
#include <open3d/Open3D.h>
#include <string>
#include <torch/extension.h>

const torch::Tensor loadNpyData(const std::string &file_path,
                                const torch::Dtype &dtype,
                                const torch::Device &device);

const Eigen::MatrixXd tensorToEigen(const torch::Tensor &tensor);

const bool saveAsTXT(const std::string &filename, const torch::Tensor &data,
                     const int &precision = 8,
                     const std::string &delimiter = " ");

const bool saveTensorAsNpy(const torch::Tensor &data,
                           const std::string &save_file_path);
