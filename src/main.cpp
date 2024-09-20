#include "pgr_params.h"
#include "reconstructor.h"

int main() {
  PGRParams pgr_params;
  const std::string pcd_file_path = "/home/chli/Downloads/bunny.ply";
  const std::string save_mesh_file_path = "/home/chli/Downloads/bunny_pgr.ply";
  const bool overwrite = true;

  Reconstructor reconstructor;
  reconstructor.reconstructSurface(pgr_params, pcd_file_path, save_mesh_file_path, overwrite);

  return 1;
}
