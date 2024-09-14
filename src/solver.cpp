#include "io.h"
#include "solver.h"
#include "cnpy.h"
#include "constant.h"
#include "kernel.h"
#include "io.h"
#include "query.h"
#include <fstream>
#include <open3d/Open3D.h>


const bool Solver::solve(
    const std::string &base,
    const std::string &query,
    const std::string &out_prefix,
    const PGRParams &pgr_params){
  const torch::Tensor y_base = loadNpyData(base, pgr_params.dtype, pgr_params.device);
  if (y_base.size(0) == 0){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t loadNpyData failed!" << std::endl;
    std::cout << "\t base : " << base << std::endl;
    return false;
  }

  const torch::Tensor x_sample = y_base.clone();

  const torch::Tensor y_base_cpu_double = y_base.cpu().toType(torch::kFloat64);

  Eigen::MatrixXd matrix;
  std::memcpy(matrix.data(), y_base_cpu_double.data_ptr<double>(), y_base_cpu_double.numel() * sizeof(double));

  open3d::geometry::KDTreeFlann base_kdtree(matrix);

  const torch::Tensor x_width = get_width(x_sample, pgr_params, base_kdtree);

  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t x_width range : [" << x_width.min().item<float>() << " , " << x_width.max().item<float>() << "], mean : " << x_width.mean().item<float>() << std::endl;

  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t start solve the system..." << std::endl;
  const torch::Tensor lse = solveLSE(x_sample, y_base, x_width, CHUNK_SIZE, TARGET_ISO_VALUE, R_SQ_STOP_EPS, pgr_params);

  const torch::Tensor out_lse = torch::cat({y_base, -lse.reshape({3, -1}).permute({1, 0})}, 1);

  const int64_t num_elements = out_lse.numel();

  std::vector<float> out_lse_array(num_elements);

  std::memcpy(out_lse_array.data(), out_lse.data_ptr<float>(), num_elements * sizeof(float));

  const std::string out_solve_npy = out_prefix + "_lse";
  if (!saveTensorAsNpy(out_lse, out_solve_npy)){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t saveTensorAsNpy failed!" << std::endl;
    std::cout << "\t out_solve_npy : " << out_solve_npy << std::endl;
    return false;
  }

  const std::string out_solve_xyz = out_prefix + "_lse.xyz";
  if (!saveAsTXT(out_solve_xyz, out_lse)){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t saveAsTXT failed!" << std::endl;
    std::cout << "\t out_solve_xyz : " << out_solve_xyz << std::endl;
    return false;
  }

  if (!pgr_params.recon_mesh) {
    return true;
  }

  const torch::Tensor q_query = loadNpyData(query, x_sample.scalar_type(), x_sample.device());
  if (q_query.size(0) == 0){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t loadNpyData failed!" << std::endl;
    std::cout << "\t query : " << query << std::endl;
    return false;
  }

  const torch::Tensor q_width = get_width(q_query, pgr_params, base_kdtree);

  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t q_width range: [ " << q_width.min().item<float>() << " , " << q_width.max().item<float>() << " ]" << std::endl;

  const torch::Tensor sample_vals = get_query_vals(x_sample, x_width, y_base, lse, CHUNK_SIZE).cpu();
  const float iso_val = torch::median(sample_vals).item<float>();
  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t sample vals range: [ " << sample_vals.min().item<float>() << " , " << sample_vals.max().item<float>() << " ], mean: " << sample_vals.mean().item<float>() << " , median: " << torch::median(sample_vals).item<float>() << std::endl;

  const std::string out_isoval_txt = out_prefix + "_isoval.txt";

  std::ofstream isoval_file(out_isoval_txt);

  if (!isoval_file.is_open()) {
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t open isoval file failed!" << std::endl;
    std::cout << "\t out_isoval_txt : " << out_isoval_txt << std::endl;
    return false;
  }

  isoval_file << std::fixed << std::setprecision(8) << iso_val;

  isoval_file.close();

  int chunk_size = 1024;
  if (pgr_params.device == torch::kCPU){
    chunk_size = 16384;
  }

  const torch::Tensor query_vals = get_query_vals(q_query, q_width, y_base, lse, chunk_size);

  const std::string out_grid_width_npy = out_prefix + "_grid_width";
  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t saving grid widths to: " << out_grid_width_npy << std::endl;
  if (!saveTensorAsNpy(q_width, out_grid_width_npy)){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t saveTensorAsNpy failed!" << std::endl;
    std::cout << "\t out_grid_width_npy : " << out_grid_width_npy << std::endl;
    return false;
  }

  const std::string out_eval_grid_npy = out_prefix + "_eval_grid";
  std::cout << "[INFO][Solver::solve]" << std::endl;
  std::cout << "\t saving grid eval values to: " << out_eval_grid_npy << std::endl;
  if (!saveTensorAsNpy(query_vals, out_eval_grid_npy)){
    std::cout << "[ERROR][Solver::solve]" << std::endl;
    std::cout << "\t saveTensorAsNpy failed!" << std::endl;
    std::cout << "\t out_eval_grid_npy : " << out_eval_grid_npy << std::endl;
    return false;
  }

  return true;
}
