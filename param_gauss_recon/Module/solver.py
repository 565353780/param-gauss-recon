import torch
import numpy as np

from param_gauss_recon.Config.constant import (
    CHUNK_SIZE,
    FLT_TYPE,
    R_SQ_STOP_EPS,
    TARGET_ISO_VALUE,
)
from param_gauss_recon.Data.pgr_params import PGRParams
from param_gauss_recon.Method.io import load_sample_from_npy
from param_gauss_recon.Method.utils import (
    get_width,
    solve,
    get_query_vals,
)


class Solver(object):
    def __init__(self) -> None:
        return

    def solve(
        self,
        base: str,
        query: str,
        output: str,
        pgr_params: PGRParams,
    ) -> bool:
        out_prefix = output

        y_base_np = np.load(base) # [N_x, 3]

        y_base = torch.from_numpy(y_base_np).type(FLT_TYPE)
        x_sample = y_base.clone()

        if pgr_params.width_min > pgr_params.width_max:
            x_width = torch.ones(x_sample.shape[0], dtype=x_sample.dtype) * pgr_params.width_min
        else:
            x_width, base_kdtree = get_width(
                x_sample,
                pgr_params,
                base_set=y_base,
                return_kdtree=True,
            )

        print("[INFO][Solver::solve]")
        print('\t x_width range: [', x_width.min().item(), ',', x_width.max().item(), '], mean: ', x_width.mean().item())

        print("[INFO][Solver::solve]")
        print('\t start solve the system...')
        solved = solve(
            x_sample,
            y_base,
            x_width,
            CHUNK_SIZE,
            TARGET_ISO_VALUE,
            R_SQ_STOP_EPS,
            pgr_params
        )
        if pgr_params.save_r:
            lse, r_list = solved
            out_r_list_txt = out_prefix + "_residuals.csv"
            np.savetxt(out_r_list_txt, r_list, fmt="%.16e", delimiter="\n")
        else:
            lse = solved

        # saving solution as npy and xyz
        out_lse_array_npy = np.concatenate(
            [y_base_np, -lse.reshape(3, -1).T], axis=1
        )
        out_solve_npy = out_prefix + "_lse"
        np.save(out_solve_npy, out_lse_array_npy)

        # saving solution as xyz
        out_solve_xyz = out_prefix + "_lse.xyz"
        np.savetxt(out_solve_xyz, out_lse_array_npy, fmt="%.8f", delimiter=" ")

        if not pgr_params.recon_mesh:
            return True

        # eval on grid
        q_query = load_sample_from_npy(query, dtype=FLT_TYPE)

        if pgr_params.width_min >= pgr_params.width_max:
            q_width = torch.ones(q_query.shape[0], dtype=FLT_TYPE) * pgr_params.width_min
        else:
            q_width = get_width(
                q_query,
                pgr_params,
                base_kdtree=base_kdtree,
                return_kdtree=False,
            )

        print(
            f"[In apps.PGRSolve] q_width range: [{q_width.min().item():.4f}, {q_width.max().item():.4f}]"
        )

        sample_vals = get_query_vals(x_sample, x_width, y_base, lse, CHUNK_SIZE)
        iso_val = float(np.median(sample_vals))
        print(
            f"[In apps.PGRSolve] sample vals range: [{sample_vals.min().item():.4f}, {sample_vals.max().item():.4f}], mean: {sample_vals.mean().item():.4f}, median: {np.median(sample_vals).item():.4f}"
        )
        out_isoval_txt = out_prefix + "_isoval.txt"
        with open(out_isoval_txt, "w") as isoval_file:
            isoval_file.write(f"{iso_val:.8f}")

        chunk_size = 1024
        if pgr_params.cpu:
            chunk_size = 16384
        query_vals = get_query_vals(q_query, q_width, y_base, lse, chunk_size)

        out_grid_width_npy = out_prefix + "_grid_width"
        print(f"[In apps.PGRSolve] Saving grid widths to {out_grid_width_npy}")
        np.save(out_grid_width_npy, q_width)

        out_eval_grid_npy = out_prefix + "_eval_grid"
        print(f"[In apps.PGRSolve] Saving grid eval values to {out_eval_grid_npy}")
        np.save(out_eval_grid_npy, query_vals)

        return True
