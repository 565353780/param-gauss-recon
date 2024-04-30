import numpy as np
from time import time
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
        sample: str,
        query: str,
        output: str,
        width_k: int,
        width_min: float,
        width_max: float,
        alpha: float,
        max_iters: Union[int, None] = None,
        cpu: bool = False,
        save_r: Union[str, None] = None,
    ) -> bool:
        if cpu:
            cp = None
        else:
            import cupy as cp

        out_prefix = output

        y_base_np = load_sample_from_npy(
            base, return_cupy=False, dtype=FLT_TYPE
        )  # [N_x, 3]
        if sample == base:
            x_sample_np = y_base_np
        else:
            x_sample_np = load_sample_from_npy(
                sample, return_cupy=False, dtype=FLT_TYPE
            )  # [N_y, 3]

        if width_min > width_max:
            x_width_np = np.ones(x_sample_np.shape[0], dtype=FLT_TYPE) * width_min
            TIME_START_X_WIDTH = 0
            TIME_END_X_WIDTH = 0
        else:
            TIME_START_X_WIDTH = time()
            x_width_np, base_kdtree = get_width(
                x_sample_np,
                k=width_k,
                dtype=FLT_TYPE,
                width_min=width_min,
                width_max=width_max,
                base_set=y_base_np,
                return_kdtree=True,
            )
            TIME_END_X_WIDTH = time()

        x_sample = x_sample_np if cpu else cp.array(x_sample_np)
        x_width = x_width_np if cpu else cp.array(x_width_np)
        y_base = y_base_np if cpu else cp.array(y_base_np)

        print(
            f"[In apps.PGRSolve] x_width range: [{x_width.min().item():.4f}, {x_width.max().item():.4f}], mean: {x_width.mean().item():.4f}"
        )
        print(
            "\033[94m"
            + f"[Timer] x_width computed in {TIME_END_X_WIDTH-TIME_START_X_WIDTH}"
            + "\033[0m"
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

        if cpu:
            lse_np = lse
        else:
            lse_np = lse.get()
            cp._default_memory_pool.free_all_blocks()

        # saving solution as npy and xyz
        out_lse_array_npy = np.concatenate(
            [y_base_np, -lse_np.reshape(3, -1).T], axis=1
        )
        out_solve_npy = out_prefix + "lse"
        np.save(out_solve_npy, out_lse_array_npy)

        # saving solution as xyz
        out_solve_xyz = out_prefix + "lse.xyz"
        np.savetxt(out_solve_xyz, out_lse_array_npy, fmt="%.8f", delimiter=" ")

        # eval on grid
        TIME_START_EVAL = time()
        q_query = load_sample_from_npy(query, return_cupy=False, dtype=FLT_TYPE)

        if width_min >= width_max:
            q_width = np.ones(q_query.shape[0], dtype=FLT_TYPE) * width_min
            TIME_START_Q_WIDTH = 0
            TIME_END_Q_WIDTH = 0
        else:
            TIME_START_Q_WIDTH = time()
            q_width = get_width(
                q_query,
                k=width_k,
                dtype=FLT_TYPE,
                width_min=width_min,
                width_max=width_max,
                base_kdtree=base_kdtree,
                return_kdtree=False,
            )
            TIME_END_Q_WIDTH = time()

        print(
            f"[In apps.PGRSolve] q_width range: [{q_width.min().item():.4f}, {q_width.max().item():.4f}]"
        )
        print(
            "\033[94m"
            + f"[Timer] q_width computed in {TIME_END_Q_WIDTH-TIME_START_Q_WIDTH}"
            + "\033[0m"
        )
        print(
            "\033[94m"
            + f"[Timer] both width computed in {TIME_END_X_WIDTH-TIME_START_X_WIDTH+TIME_END_Q_WIDTH-TIME_START_Q_WIDTH}"
            + "\033[0m"
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

        TIME_END_EVAL = time()
        print(
            "\033[94m"
            + f"[Timer] Eval on grid finished in {(TIME_END_EVAL-TIME_START_EVAL)-(TIME_END_Q_WIDTH-TIME_START_Q_WIDTH)}"
            + "\033[0m"
        )
        return True
