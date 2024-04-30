import argparse

from param_gauss_recon.Module.reconstructor import Reconstructor

parser = argparse.ArgumentParser()
parser.add_argument(
    "input",
    help="xyz format point cloud input, this should be the one to construct the final query octree",
)
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
parser.add_argument("-m", "--max_iters", type=int, help="maximum iterations for CG")
parser.add_argument(
    "-d", "--max_depth", type=int, default=10, help="max depth of octree"
)
parser.add_argument(
    "-md", "--min_depth", type=int, default=1, help="min depth of octree"
)
parser.add_argument("--cpu", action="store_true", help="run with cpu")
parser.add_argument("--save_r", action="store_true", help="save the residual list")
args = parser.parse_args()


reconstructor = Reconstructor()
reconstructor.reconstructSurface(
    args.input,
    args.width_k,
    args.width_max,
    args.width_min,
    args.alpha,
    args.max_iters,
    args.max_depth,
    args.min_depth,
    args.cpu,
    args.save_r,
)
