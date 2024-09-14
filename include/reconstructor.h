#pragma once

#include "pcd_sampler.h"
#include "pgr_params.h"
#include "solver.h"

class Reconstructor {
public:
  Reconstructor(){};

  const std::string toSampledPcdFile(
      const std::string &input,
      const int &sample_point_num = -1);

  const bool reconstructSurface(
      const std::string &input,
      PGRParams &pgr_params);

public:
  std::string param_midfix;

private:
  PcdSampler pcd_sampler_;
  Solver solver_;
};
