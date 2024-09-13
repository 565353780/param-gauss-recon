import torch
import numpy as np
from typing import Union

from param_gauss_recon.Config.constant import (
    CHUNK_SIZE,
    FLT_TYPE,
    R_SQ_STOP_EPS,
    TARGET_ISO_VALUE,
)
from param_gauss_recon.Method.utils import (
    get_width,
    load_sample_from_npy,
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
        width_k: int,
        width_min: float,
        width_max: float,
        alpha: float,
        max_iters: Union[int, None] = None,
        cpu: bool = False,
        save_r: Union[str, None] = None,
        recon_mesh: bool=True,
    ) -> bool:
        out_prefix = output

        y_base_np = np.load(base) # [N_x, 3]

        y_base = torch.from_numpy(y_base_np).type(FLT_TYPE)
        x_sample = y_base.clone()

        if width_min > width_max:
            x_width = torch.ones(x_sample.shape[0], dtype=x_sample.dtype) * width_min
        else:
            x_width, base_kdtree = get_width(
                x_sample,
                k=width_k,
                width_min=width_min,
                width_max=width_max,
                base_set=y_base,
                return_kdtree=True,
            )

        print(
            f"[In apps.PGRSolve] x_width range: [{x_width.min().item():.4f}, {x_width.max().item():.4f}], mean: {x_width.mean().item():.4f}"
        )

        print("[In apps.PGRSolve] Starting to solve the system...")
        solved = solve(
            x_sample,
            y_base,
            x_width,
            chunk_size=CHUNK_SIZE,
            dtype=FLT_TYPE,
            iso_value=TARGET_ISO_VALUE,
            r_sq_stop_eps=R_SQ_STOP_EPS,
            alpha=alpha,
            max_iters=max_iters,
            save_r=save_r,
        )
        if save_r:
            lse, r_list = solved
            out_r_list_txt = out_prefix + "residuals.csv"
            np.savetxt(out_r_list_txt, r_list, fmt="%.16e", delimiter="\n")
        else:
            lse = solved

        # saving solution as npy and xyz
        out_lse_array_npy = np.concatenate(
            [y_base_np, -lse.reshape(3, -1).T], axis=1
        )
        out_solve_npy = out_prefix + "lse"
        np.save(out_solve_npy, out_lse_array_npy)

        # saving solution as xyz
        out_solve_xyz = out_prefix + "lse.xyz"
        np.savetxt(out_solve_xyz, out_lse_array_npy, fmt="%.8f", delimiter=" ")

        if not recon_mesh:
            return True

        # eval on grid
        q_query = load_sample_from_npy(query, dtype=FLT_TYPE)

        if width_min >= width_max:
            q_width = np.ones(q_query.shape[0], dtype=FLT_TYPE) * width_min
        else:
            q_width = get_width(
                q_query,
                k=width_k,
                width_min=width_min,
                width_max=width_max,
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
        out_isoval_txt = out_prefix + "isoval.txt"
        with open(out_isoval_txt, "w") as isoval_file:
            isoval_file.write(f"{iso_val:.8f}")

        chunk_size = 1024
        if cpu:
            chunk_size = 16384
        query_vals = get_query_vals(q_query, q_width, y_base, lse, chunk_size)

        out_grid_width_npy = out_prefix + "grid_width"
        print(f"[In apps.PGRSolve] Saving grid widths to {out_grid_width_npy}")
        np.save(out_grid_width_npy, q_width)

        out_eval_grid_npy = out_prefix + "eval_grid"
        print(f"[In apps.PGRSolve] Saving grid eval values to {out_eval_grid_npy}")
        np.save(out_eval_grid_npy, query_vals)

        return True
