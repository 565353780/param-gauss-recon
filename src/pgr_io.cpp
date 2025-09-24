#include "pgr_io.h"
#include <filesystem>
#include <fstream>
#include <iomanip> // For setting float precision
#include <iostream>

const torch::Tensor loadNpyData(const std::string &file_path,
                                const torch::Dtype &dtype,
                                const torch::Device &device) {
  if (!std::filesystem::exists(file_path)) {
    std::cout << "[ERROR][pgr_io::loadNpyData]" << std::endl;
    std::cout << "\t file not exist!" << std::endl;
    std::cout << "\t file_path : " << file_path << std::endl;
    return torch::empty(0);
  }

  cnpy::NpyArray data = cnpy::npy_load(file_path);
  float *data_ptr = data.data<float>();
  const std::vector<size_t> &shape = data.shape;
  const std::vector<int64_t> tensor_shape(shape.begin(), shape.end());

  const torch::TensorOptions opts =
      torch::TensorOptions().dtype(dtype).device(torch::kCPU);
  const torch::Tensor data_tensor =
      torch::from_blob(data_ptr, tensor_shape, opts).clone().to(device);

  return data_tensor;
}

const Eigen::MatrixXd tensorToEigen(const torch::Tensor &tensor) {
  TORCH_CHECK(tensor.dim() == 2 && tensor.size(1) == 3,
              "Input tensor must be of shape [N, 3]");

  torch::Tensor tensor_cpu = tensor.to(torch::kCPU).to(torch::kFloat64);

  const double *data_ptr = tensor_cpu.data_ptr<double>();

  Eigen::MatrixXd eigen_matrix =
      Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                     Eigen::RowMajor>>(data_ptr, tensor.size(0),
                                                       tensor.size(1));

  return eigen_matrix;
}

const bool saveAsTXT(const std::string &filename, const torch::Tensor &data,
                     const int &precision, const std::string &delimiter) {
  const std::string file_folder_path =
      std::filesystem::path(filename).parent_path();
  if (!std::filesystem::exists(file_folder_path)) {
    std::filesystem::create_directories(file_folder_path);
  }

  std::ofstream out_file(filename);

  if (!out_file.is_open()) {
    std::cout << "[ERROR][pgr_io::saveAsTXT]" << std::endl;
    std::cout << "\t open out file failed!" << std::endl;
    std::cout << "\t out_file : " << filename << std::endl;
    return false;
  }

  out_file << std::fixed << std::setprecision(precision);

  const torch::Tensor cpu_data = data.cpu();

  for (int i = 0; i < cpu_data.size(0); ++i) {
    for (int j = 0; j < cpu_data.size(1); ++j) {
      out_file << cpu_data[i][j];

      if (j != cpu_data.size(1) - 1) {
        out_file << delimiter;
      }
    }
    out_file << "\n";
  }

  out_file.close();

  return true;
}

const bool saveTensorAsNpy(const torch::Tensor &data,
                           const std::string &save_file_path) {
  std::string valid_save_file_path = save_file_path;
  if (valid_save_file_path.find(".npy") == std::string::npos) {
    valid_save_file_path += ".npy";
  }

  const std::string save_folder_path =
      std::filesystem::path(valid_save_file_path).parent_path();
  if (!std::filesystem::exists(save_folder_path)) {
    std::filesystem::create_directories(save_folder_path);
  }

  const int64_t num_elements = data.numel();

  std::vector<float> data_array(num_elements);

  const torch::Tensor cpu_float_data = data.cpu().toType(torch::kFloat32);

  std::memcpy(data_array.data(), cpu_float_data.data_ptr<float>(),
              num_elements * sizeof(float));

  const std::vector<size_t> shape = std::vector<size_t>(
      cpu_float_data.sizes().begin(), cpu_float_data.sizes().end());

  cnpy::npy_save(valid_save_file_path, &data_array[0], shape, "w");

  return true;
}

const bool createFileFolder(const std::string &file_path) {
  std::filesystem::path dir_path =
      std::filesystem::path(file_path).parent_path();

  if (std::filesystem::exists(dir_path)) {
    return true;
  }

  std::filesystem::create_directories(dir_path);

  return true;
}

const bool savePcdFile(const std::vector<double> &vertices,
                       const std::string &file_path) {
  if (vertices.size() % 3 != 0) {
    std::cerr << "Error: vertices vector size should be a multiple of 3!"
              << std::endl;
    return false;
  }

  open3d::geometry::PointCloud point_cloud;

  for (size_t i = 0; i < vertices.size(); i += 3) {
    Eigen::Vector3d point(vertices[i], vertices[i + 1], vertices[i + 2]);
    point_cloud.points_.push_back(point);
  }

  createFileFolder(file_path);

  return open3d::io::WritePointCloud(file_path, point_cloud, true);
}

MeshData readPcdData(const std::string &ply_file) {
  MeshData mesh_data;

  // 加载三角网格
  open3d::geometry::PointCloud pcd;
  if (!open3d::io::ReadPointCloud(ply_file, pcd)) {
    std::cerr << "Failed to read the mesh file." << std::endl;
    return mesh_data;
  }

  // 获取顶点数据
  const auto &vertices = pcd.points_;

  // 填充 MeshData 结构体
  mesh_data.vertices.reserve(vertices.size() * 3);
  for (const auto &vertex : vertices) {
    mesh_data.vertices.push_back(vertex[0]);
    mesh_data.vertices.push_back(vertex[1]);
    mesh_data.vertices.push_back(vertex[2]);
  }

  return mesh_data;
}

MeshData readMeshData(const std::string &ply_file) {
  MeshData mesh_data;

  // 加载三角网格
  open3d::geometry::TriangleMesh mesh;
  if (!open3d::io::ReadTriangleMesh(ply_file, mesh)) {
    std::cerr << "Failed to read the mesh file." << std::endl;
    return mesh_data;
  }

  // 估计法线
  mesh.ComputeVertexNormals();

  // 获取顶点数据
  const auto &vertices = mesh.vertices_;
  const auto &triangles = mesh.triangles_;
  const auto &normals = mesh.vertex_normals_;

  // 填充 MeshData 结构体
  mesh_data.vertices.reserve(vertices.size() * 3);
  for (const auto &vertex : vertices) {
    mesh_data.vertices.push_back(vertex[0]);
    mesh_data.vertices.push_back(vertex[1]);
    mesh_data.vertices.push_back(vertex[2]);
  }

  mesh_data.indices.reserve(triangles.size() * 3);
  for (const auto &triangle : triangles) {
    mesh_data.indices.push_back(triangle[0]);
    mesh_data.indices.push_back(triangle[1]);
    mesh_data.indices.push_back(triangle[2]);
  }

  mesh_data.normals.reserve(normals.size() * 3);
  for (const auto &normal : normals) {
    mesh_data.normals.push_back(normal[0]);
    mesh_data.normals.push_back(normal[1]);
    mesh_data.normals.push_back(normal[2]);
  }

  return mesh_data;
}
