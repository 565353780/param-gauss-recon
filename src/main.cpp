#include "pgr_params.h"
#include "reconstructor.h"

int main() {
  PGRParams pgr_params;
  const std::string pcd_file_path = "/home/chli/Downloads/bunny.ply";

  Reconstructor reconstructor;
  reconstructor.reconstructSurface(pcd_file_path, pgr_params);

  return 1;
}
