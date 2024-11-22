#pragma once

#include "pcd_sampler.h"
#include "pgr_params.h"
#include "pgr_solver.h"

class PGRReconstructor {
public:
  PGRReconstructor() { reset(); };

  const bool reset();

  const std::string toSampledPcdFile(const std::string &input,
                                     const int &sample_point_num = -1);

  const bool toNormalizedSampledPcdFile(
      const std::string &input, const int &sample_point_num = -1,
      const std::string &save_normalize_pcd_file_path = "",
      const bool &overwrite = false);

  const bool toInvertNormalizedMeshFile(const std::string &mesh_file_path,
                                        const std::string &save_mesh_file_path,
                                        const bool &overwrite = false);

  const bool reconstructSurface(PGRParams &pgr_params, const std::string &input,
                                const std::string &save_normalize_pcd_file_path,
                                const std::string &save_mesh_file_path = "",
                                const bool &overwrite = false);

public:
  std::string param_midfix;

private:
  PcdSampler pcd_sampler_;
  PGRSolver pgr_solver_;

  Eigen::Vector3d translate_;
  Eigen::Vector3d scale_;
  Eigen::Vector3d scale_inverse_;
};
