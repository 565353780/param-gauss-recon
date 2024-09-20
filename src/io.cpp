#include "io.h"
#include <filesystem>
#include <fstream>
#include <iomanip> // For setting float precision
#include <iostream>

const torch::Tensor loadNpyData(const std::string &file_path,
                                const torch::Dtype &dtype,
                                const torch::Device &device) {
  if (!std::filesystem::exists(file_path)) {
    std::cout << "[ERROR][io::loadNpyData]" << std::endl;
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
    std::cout << "[ERROR][io::saveAsTXT]" << std::endl;
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
