#include "constant.h"
#include "kernel.h"

using namespace torch::indexing;

const torch::Tensor get_A(
    const torch::Tensor &x,
    const torch::Tensor &y,
    const torch::Tensor &x_width){
  const int N_query = x.size(0);

  torch::Tensor A = x.unsqueeze(1) - y.unsqueeze(0);

  torch::Tensor dist = torch::sqrt((A * A).sum(-1));
  dist.index_put_({dist == 0.0}, EPSILON);

  const torch::Tensor x_width_unsqueezed = x_width.unsqueeze(1);
  const torch::Tensor inv_dist = torch::where(dist > x_width_unsqueezed, 1.0 / dist, 1.0 / x_width_unsqueezed);
  const torch::Tensor inv_cub_dist = inv_dist * inv_dist * inv_dist / 4.0 / M_PI;

  A *= inv_cub_dist.unsqueeze(-1);
  A = A.permute({0, 2, 1});
  A = A.reshape({N_query, -1});

  return -A;
}

const torch::Tensor mul_A_T(
    const torch::Tensor &x,
    const torch::Tensor &y,
    const torch::Tensor &xi,
    const torch::Tensor &x_width,
    const int &chunk_size){
  const int N_sample = y.size(0);
  const int n_y_chunks = std::floor(N_sample / chunk_size) + 1;

  const torch::TensorOptions opts = torch::TensorOptions().dtype(x.dtype()).device(x.device());
  torch::Tensor lse = torch::zeros({3, N_sample}, opts);
  for (int i = 0; i < n_y_chunks; ++i){
    const Slice chunk_slice = Slice(i * chunk_size, (i + 1) * chunk_size);
    const torch::Tensor y_chunk = y.index({chunk_slice});
    const torch::Tensor A_chunk = get_A(x, y_chunk, x_width);

    const torch::Tensor lse_chunk_results = torch::einsum("jk,j", {A_chunk, xi}).reshape({3, -1});
    for (int j = 0; j < 3; ++j){
      lse.index_put_({j, chunk_slice}, lse_chunk_results[j]);
    }
  }

  return lse.reshape({-1});
}

const torch::Tensor get_B(
    const torch::Tensor &x,
    const torch::Tensor &y,
    const int &chunk_size,
    const torch::Tensor &x_width,
    const float &alpha){
  const int N_query = x.size(0);

  const torch::TensorOptions opts = torch::TensorOptions().dtype(x.dtype()).device(x.device());
  const torch::TensorOptions bool_opts = torch::TensorOptions().dtype(torch::kBool).device(x.device());
  torch::Tensor B = torch::zeros({N_query, N_query}, opts);

  const int n_row_chunks = std::floor(N_query / chunk_size) + 1;
  const int n_col_chunks = n_row_chunks;

  for (int i = 0; i < n_row_chunks; ++i) {
    const Slice row_chunk_slice = Slice(i * chunk_size, (i + 1) * chunk_size);
    const torch::Tensor x_chunk_i = x.index({row_chunk_slice});

    if (x_chunk_i.size(0) <= 0){
      continue;
    }

    const torch::Tensor x_chunk_eps_i = x_width.index({row_chunk_slice});
    const torch::Tensor A_block_i = get_A(x_chunk_i, y, x_chunk_eps_i);

    for (int j = i; j < n_col_chunks; ++j){
      const Slice col_chunk_slice = Slice(j * chunk_size, (j + 1) * chunk_size);
      const torch::Tensor x_chunk_j = x.index({col_chunk_slice});

      if (x_chunk_j.size(0) <= 0){
        continue;
      }

      const torch::Tensor x_chunk_eps_j = x_width.index({col_chunk_slice});
      const torch::Tensor A_block_j = get_A(x_chunk_j, y, x_chunk_eps_j);

      B.index_put_({row_chunk_slice, col_chunk_slice}, torch::einsum("ik,jk->ij", {A_block_i, A_block_j}));

      if (i != j){
        B.index_put_({col_chunk_slice, row_chunk_slice}, B.index({row_chunk_slice, col_chunk_slice}).transpose(1, 0));
      }
      else{
        const int block_size = B.index({row_chunk_slice, col_chunk_slice}).size(0);
        if (block_size <= 0){
          continue;
        }

        const torch::Tensor diag_mask = torch::eye(block_size, bool_opts);
        torch::Tensor B_block = B.index({row_chunk_slice, col_chunk_slice});
        B_block.index_put_({diag_mask}, B_block.index({diag_mask}) * alpha);
        B.index_put_({row_chunk_slice, col_chunk_slice}, B_block);
      }
    }
  }

  return B;
}

const torch::Tensor solve(
    const torch::Tensor &x,
    const torch::Tensor &y,
    const torch::Tensor &x_width,
    const int &chunk_size,
    const float &iso_value,
    const float &r_sq_stop_eps,
    const PGRParams &pgr_params){
  std::cout << "[INFO][kernel::solve]" << std::endl;
  std::cout << "\t start pre-computing B..." << std::endl;
  const torch::Tensor B = get_B(x, y, chunk_size, x_width, pgr_params.alpha);

  const torch::TensorOptions opts = torch::TensorOptions().dtype(x.dtype()).device(x.device());
  torch::Tensor xi = torch::zeros({x.size(0)}, opts);
  torch::Tensor r = torch::ones({x.size(0)}, opts) * iso_value;
  torch::Tensor p = r.clone();

  std::cout << "[INFO][kernel::solve]" << std::endl;
  std::cout << "\t start CG iterations..." << std::endl;
  for (int i = 0; i < y.size(0); ++i){
    const torch::Tensor Bp = torch::matmul(B, p);
    const float r_sq = r.dot(r).item<float>();

    const float alpha = r_sq / p.dot(Bp).item<float>();
    xi += alpha * p;
    r -= alpha * Bp;
    const float beta = r.dot(r).item<float>() / r_sq;
    p *= beta;
    p += r;

    if (r_sq < r_sq_stop_eps){
      break;
    }
  }

  const torch::Tensor lse = mul_A_T(x, y, xi, x_width, chunk_size);

  return lse;
}
