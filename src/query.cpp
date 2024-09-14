#include "query.h"
#include "kernel.h"

using namespace torch::indexing;

const torch::Tensor get_query_vals(
    const torch::Tensor &queries,
    const torch::Tensor &q_width,
    const torch::Tensor &y_base,
    const torch::Tensor &lse,
    const int &chunk_size){
  const int split_length = std::floor(queries.size(0) / chunk_size) + 1;
  const int split_num = std::floor(queries.size(0) / split_length);

  const std::vector<torch::Tensor> query_chunks = torch::split(queries, split_num);
  const std::vector<torch::Tensor> q_cut_chunks = torch::split(q_width, split_num);
  std::vector<torch::Tensor> query_vals_vec;

  std::cout << "[INFO][query::get_query_vals]" << std::endl;
  std::cout << "\t start query on the grid..." << std::endl;
  for (int i = 0; i < query_chunks.size(); ++i){
    const torch::Tensor chunk = query_chunks[i];
    const torch::Tensor cut_chunk = q_cut_chunks[i];

    const torch::Tensor A_show = get_A(chunk, y_base, cut_chunk);
    query_vals_vec.emplace_back(torch::matmul(A_show, lse));
  }

  const torch::Tensor query_vals = torch::cat(query_vals_vec, 0);

  return query_vals;
}

const torch::Tensor get_width(
    const torch::Tensor &query_set,
    const PGRParams &pgr_params,
    const open3d::geometry::KDTreeFlann &base_kdtree){
  const torch::TensorOptions opts = torch::TensorOptions().dtype(query_set.dtype()).device(query_set.device());

  if (pgr_params.width_min > pgr_params.width_max){
    const torch::Tensor x_width = torch::ones({query_set.size(0)}, opts) * pgr_params.width_min;

    return x_width;
  }

  const int search_point_num = pgr_params.width_k + 2;

  torch::Tensor x_knn_dist = torch::zeros({query_set.size(0), search_point_num}, opts);

  for (int i = 0; i < query_set.size(0); ++i){
    std::vector<int> indices(search_point_num);
    std::vector<double> distances(search_point_num);
    const Eigen::Vector3d query_point(query_set[i][0].item<float>(), query_set[i][1].item<float>(), query_set[i][2].item<float>());
    base_kdtree.SearchKNN(query_point, search_point_num, indices, distances);

    for (int j = 0; j < search_point_num; ++j){
      x_knn_dist[i][j] = float(distances[j]);
    }
  }

  const torch::Tensor x_knn_dist_block = x_knn_dist.index({None, Slice(1, None)});

  torch::Tensor x_width = torch::sqrt(torch::einsum("nk,nk->n", {x_knn_dist_block, x_knn_dist_block}) / pgr_params.width_k);
  torch::clip(x_width, pgr_params.width_min, pgr_params.width_max);

  return x_width;
}
