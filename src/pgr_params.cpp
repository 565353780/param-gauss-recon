#include "pgr_params.h"

const bool PGRParams::update(){
  if (torch::cuda::is_available()){
    device = torch::kCUDA;
  }
  else{
    if (device == torch::kCPU){
      return true;
    }

    std::cout << "[WARN][PGRParams::update]" << std::endl;
    std::cout << "\t torch cuda is not available! will use cpu!" << std::endl;
    device = torch::kCPU;
  }

  return true;
}
