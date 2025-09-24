#pragma once

#include <Eigen/Core>
#include <cnpy.h>
#include <filesystem>
#include <open3d/Open3D.h>
#include <string>
#include <torch/extension.h>

struct MeshData {
  std::vector<double> vertices;      // xyz
  std::vector<size_t> indices;       // triangles [v1, v2, v3, ...]
  std::vector<double> normals;       // xyz
  std::vector<size_t> cornerIndices; // 拐角点id
};

const torch::Tensor loadNpyData(const std::string &file_path,
                                const torch::Dtype &dtype,
                                const torch::Device &device);

const Eigen::MatrixXd tensorToEigen(const torch::Tensor &tensor);

const bool saveAsTXT(const std::string &filename, const torch::Tensor &data,
                     const int &precision = 8,
                     const std::string &delimiter = " ");

const bool saveTensorAsNpy(const torch::Tensor &data,
                           const std::string &save_file_path);

const bool createFileFolder(const std::string &file_path);

const bool savePcdFile(const std::vector<double> &vertices,
                       const std::string &file_path);

MeshData readPcdData(const std::string &ply_file);

MeshData readMeshData(const std::string &ply_file);
