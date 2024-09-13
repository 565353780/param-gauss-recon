import torch
import numpy as np

from param_gauss_recon.Config.constant import (
    CHUNK_SIZE,
    R_SQ_STOP_EPS,
    TARGET_ISO_VALUE,
)
from param_gauss_recon.Data.pgr_params import PGRParams
from param_gauss_recon.Method.io import loadNpyData
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

        y_base = loadNpyData(base, pgr_params.dtype, pgr_params.device) # [N_x, 3]
        if y_base is None:
            print('[ERROR][Solver::solve]')
            print('\t loadNpyData failed!')
            print('\t base :', base)
            return False

        x_sample = y_base.clone()

        x_width, base_kdtree = get_width(x_sample, pgr_params, y_base)

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
        out_lse = torch.cat([y_base, -lse.reshape(3, -1).T], dim=1)
        out_lse_array = out_lse.cpu().numpy()
        out_solve_npy = out_prefix + "_lse"
        np.save(out_solve_npy, out_lse_array)

        # saving solution as xyz
        out_solve_xyz = out_prefix + "_lse.xyz"
        np.savetxt(out_solve_xyz, out_lse_array, fmt="%.8f", delimiter=" ")

        if not pgr_params.recon_mesh:
            return True

        # eval on grid
        q_query = loadNpyData(query, x_sample.dtype, x_sample.device)
        if q_query is None:
            print('[ERROR][Solver::solve]')
            print('\t loadNpyData failed!')
            print('\t q_query :', q_query)
            return False

        q_width, _ = get_width(q_query, pgr_params, base_kdtree=base_kdtree)

        print(
            f"[In apps.PGRSolve] q_width range: [{q_width.min().item():.4f}, {q_width.max().item():.4f}]"
        )

        sample_vals = get_query_vals(x_sample, x_width, y_base, lse, CHUNK_SIZE).cpu().numpy()
        iso_val = float(np.median(sample_vals))
        print(
            f"[In apps.PGRSolve] sample vals range: [{sample_vals.min():.4f}, {sample_vals.max():.4f}], mean: {sample_vals.mean():.4f}, median: {np.median(sample_vals):.4f}"
        )
        out_isoval_txt = out_prefix + "_isoval.txt"
        with open(out_isoval_txt, "w") as isoval_file:
            isoval_file.write(f"{iso_val:.8f}")

        chunk_size = 1024
        if pgr_params.device == 'cpu':
            chunk_size = 16384
        query_vals = get_query_vals(q_query, q_width, y_base, lse, chunk_size)

        out_grid_width_npy = out_prefix + "_grid_width"
        print(f"[In apps.PGRSolve] Saving grid widths to {out_grid_width_npy}")
        np.save(out_grid_width_npy, q_width.cpu().numpy())

        out_eval_grid_npy = out_prefix + "_eval_grid"
        print(f"[In apps.PGRSolve] Saving grid eval values to {out_eval_grid_npy}")
        np.save(out_eval_grid_npy, query_vals.cpu().numpy())

        return True
