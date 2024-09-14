#include "io.h"
#include <filesystem>

const torch::Tensor loadNpyData(
    const std::string &file_path,
    const torch::Dtype &dtype,
    const torch::Device &device){
  if (!std::filesystem::exists(file_path)){
    std::cout << "[ERROR][io::loadNpyData]" << std::endl;
    std::cout << "\t file not exist!" << std::endl;
    std::cout << "\t file_path : " << file_path << std::endl;
    return torch::empty(0);
  }

  cnpy::NpyArray data = cnpy::npy_load(file_path);
  float *data_ptr = data.data<float>();
  const std::vector<size_t> &shape = data.shape;
  const std::vector<int64_t> tensor_shape(shape.begin(), shape.end());

  const torch::TensorOptions opts = torch::TensorOptions().dtype(dtype).device(device);
  const torch::Tensor data_tensor = torch::from_blob(data_ptr, tensor_shape, opts).clone();

  return data_tensor;
}
