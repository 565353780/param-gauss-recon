#include "pgr_params.h"

const bool PGRParams::update() {
  if (torch::cuda::is_available()) {
    device = torch::kCUDA;
  } else {
    if (device == torch::kCPU) {
      return true;
    }

    std::cout << "[WARN][PGRParams::update]" << std::endl;
    std::cout << "\t torch cuda is not available! will use cpu!" << std::endl;
    device = torch::kCPU;
  }

  return true;
}

const std::string PGRParams::toCMDStr() {
  std::string cmd_str = "";

  cmd_str += " -d " + std::to_string(max_depth);
  cmd_str += " -m " + std::to_string(min_depth);

  return cmd_str;
}

const std::string PGRParams::toLogStr() {
  std::string log_str = "";

  log_str += "_k_" + std::to_string(width_k);
  log_str += "_min_" + std::to_string(width_min);
  log_str += "_max_" + std::to_string(width_max);
  log_str += "_alpha_" + std::to_string(alpha);
  log_str += "_depth_min_" + std::to_string(min_depth);
  log_str += "_depth_max_" + std::to_string(min_depth);

  return log_str;
}

const bool PGRParams::outputInfo(const int &info_level) {
  std::string line_start = "";
  for (int i = 0; i < info_level; ++i) {
    line_start += "\t";
  }

  std::cout << line_start << "---------Settings---------" << std::endl;
  std::cout << line_start << "min depth:   " << std::to_string(min_depth)
            << std::endl;
  std::cout << line_start << "max depth:   " << std::to_string(max_depth)
            << std::endl;
  std::cout << line_start << "width_k:     " << std::to_string(width_k)
            << std::endl;
  std::cout << line_start << "width_min:   " << std::to_string(width_min)
            << std::endl;
  std::cout << line_start << "width_max:   " << std::to_string(width_max)
            << std::endl;
  std::cout << line_start << "alpha:       " << std::to_string(alpha)
            << std::endl;
  std::cout << line_start << "---------Settings---------" << std::endl;
  return true;
}
