import argparse

from param_gauss_recon.Data.pgr_params import PGRParams
from param_gauss_recon.Module.reconstructor import Reconstructor

parser = argparse.ArgumentParser()
parser.add_argument(
    "input",
    help="xyz format point cloud input, this should be the one to construct the final query octree",
)
parser.add_argument(
    "tmp_folder_path",
    help="tmp reuslts save folder path",
)
parser.add_argument(
    "save_mesh_file_path",
    help="final recon mesh file save path",
)
parser.add_argument("--cpu", action="store_true", help="Use cpu only")
parser.add_argument("-s", "--sample", type=int, default=20000, help="sample point num")
parser.add_argument(
    "-wk", "--width_k", type=int, default=7, help="k in knn for width estimation"
)
parser.add_argument(
    "-wmax",
    "--width_max",
    type=float,
    default=0.015,
    help="minimum width, overrides --width_max",
)
parser.add_argument(
    "-wmin", "--width_min", type=float, default=0.0015, help="maximum width"
)
parser.add_argument(
    "-a", "--alpha", type=float, default=1.05, help="alpha for regularization"
)
parser.add_argument(
    "-d", "--max_depth", type=int, default=10, help="max depth of octree"
)
parser.add_argument(
    "-md", "--min_depth", type=int, default=1, help="min depth of octree"
)
args = parser.parse_args()

pgr_params = PGRParams()
pgr_params.sample_point_num = args.sample
pgr_params.width_k = args.width_k
pgr_params.width_min = args.width_min
pgr_params.width_max = args.width_max
pgr_params.alpha = args.alpha
pgr_params.min_depth = args.min_depth
pgr_params.max_depth = args.max_depth
pgr_params.device = "cpu" if args.cpu else "cuda"

reconstructor = Reconstructor()
reconstructor.reconstructSurface(
    args.input,
    pgr_params,
    args.tmp_folder_path,
    args.save_mesh_file_path,
)
