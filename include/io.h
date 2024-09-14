#pragma once

#include <cnpy.h>
#include <string>
#include <torch/extension.h>

const torch::Tensor loadNpyData(
    const std::string &file_path,
    const torch::Dtype &dtype,
    const torch::Device &device);
