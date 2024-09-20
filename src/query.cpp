#include "query.h"
#include "kernel.h"

using namespace torch::indexing;

const torch::Tensor get_query_vals(const torch::Tensor &queries,
                                   const torch::Tensor &q_width,
                                   const torch::Tensor &y_base,
                                   const torch::Tensor &lse,
                                   const int &chunk_size) {
  const int split_length = std::floor(queries.size(0) / chunk_size) + 1;
  const int split_num = std::floor(queries.size(0) / split_length);

  const std::vector<torch::Tensor> query_chunks =
      torch::split(queries, split_num);
  const std::vector<torch::Tensor> q_cut_chunks =
      torch::split(q_width, split_num);
  std::vector<torch::Tensor> query_vals_vec;

  std::cout << "[INFO][query::get_query_vals]" << std::endl;
  std::cout << "\t start query on the grid..." << std::endl;
  for (int i = 0; i < query_chunks.size(); ++i) {
    const torch::Tensor chunk = query_chunks[i];
    const torch::Tensor cut_chunk = q_cut_chunks[i];

    const torch::Tensor A_show = get_A(chunk, y_base, cut_chunk);
    query_vals_vec.emplace_back(torch::matmul(A_show, lse));
  }

  const torch::Tensor query_vals = torch::cat(query_vals_vec, 0);

  return query_vals;
}

const torch::Tensor get_width(const torch::Tensor &query_set,
                              const PGRParams &pgr_params,
                              KDTree &base_kdtree) {
  const torch::TensorOptions opts = torch::TensorOptions()
                                        .dtype(query_set.dtype())
                                        .device(query_set.device());

  if (pgr_params.width_min > pgr_params.width_max) {
    const torch::Tensor x_width =
        torch::ones({query_set.size(0)}, opts) * pgr_params.width_min;

    return x_width;
  }

  const int search_point_num = pgr_params.width_k + 2;

  torch::Tensor x_knn_dist =
      torch::zeros({query_set.size(0), search_point_num}, opts);

  std::cout << "[INFO][query::get_width]" << std::endl;
  std::cout << "\t start query KDTree..." << std::endl;
  for (int i = 0; i < query_set.size(0); ++i) {
    if ((i + 1) % 100 == 0) {
      std::cout << "\r\t\t iter : " << (i + 1) << " / " << query_set.size(0);
    }

    std::vector<float> query_point = {query_set[i][0].item<float>(),
                                      query_set[i][1].item<float>(),
                                      query_set[i][2].item<float>()};

    std::vector<size_t> indices(search_point_num);
    std::vector<float> distances(search_point_num);

    KNNResultSet<float> resultSet(search_point_num);
    resultSet.init(&indices[0], &distances[0]);

    base_kdtree.findNeighbors(resultSet, &query_point[0], SearchParameters(10));

    for (int j = 0; j < search_point_num; ++j) {
      x_knn_dist[i][j] = float(distances[j]);
    }
  }
  std::cout << std::endl;

  x_knn_dist = torch::sqrt(x_knn_dist);

  const torch::Tensor x_knn_dist_block =
      x_knn_dist.index({Slice(), Slice(1, None)});

  torch::Tensor x_width = torch::sqrt(
      torch::einsum("nk,nk->n", {x_knn_dist_block, x_knn_dist_block}) /
      pgr_params.width_k);
  torch::clip(x_width, pgr_params.width_min, pgr_params.width_max);

  return x_width;
}
