#pragma once

#include "pgr_params.h"
#include <string>

class Solver {
public:
  Solver(){};

  const bool solve(
      const std::string &base,
      const std::string &query,
      const std::string &out_prefix,
      const PGRParams &pgr_params);
};
