#pragma once

#include <torch/extension.h>

class PGRParams {
public:
  PGRParams(){};

  const bool update();

public:
  int sample_point_num = 2000;
  int width_k = 7;
  float width_min = 0.0015;
  float width_max = 0.015;
  float alpha = 1.05;
  int min_depth = 1;
  int max_depth = 10;
  torch::Dtype dtype = torch::kFloat32;
  torch::Device device = torch::kCUDA;
  bool recon_mesh = true;
};
