import os
import shutil
from time import time
from typing import Union
from os.path import join, basename

from param_gauss_recon.Config.path import EXPORT_QUERY_EXE, LOAD_QUERY_EXE
from param_gauss_recon.Module.solver import Solver


class Reconstructor(object):
    def __init__(self) -> None:
        self.solver = Solver()
        return

    def reconstructSurface(
        self,
        input: str,
        width_k: int = 7,
        width_max: float = 0.015,
        width_min: float = 0.0015,
        alpha: float = 1.05,
        max_iters: Union[int, None] = None,
        max_depth: int = 10,
        min_depth: int = 1,
        save_r: Union[str, None] = None,
    ) -> bool:
        PARAM_MIDFIX = f"_k_{width_k}_min_{width_min}_max_{width_max}_alpha_{alpha}_depth_min_{min_depth}_depth_max_{min_depth}_"
        setup_str = (
            "---------Settings---------\n"
            + f"min depth:   {min_depth}\n"
            + f"max depth:   {max_depth}\n"
            + f"max iters:   {max_iters}\n"
            + f"width_k:     {width_k}\n"
            + f"width_min:   {width_min}\n"
            + f"width_max:   {width_max}\n"
            + f"alpha:       {alpha}\n"
            + "---------Settings---------"
        )

        print(setup_str)

        in_filename = input
        data_index = in_filename.split("/")[-1][:-4]

        results_folder = "results/"
        sample_file_folder = results_folder + data_index + "/samples/"
        solve_file_folder = results_folder + data_index + "/solve/"
        recon_file_folder = results_folder + data_index + "/recon/"

        sample_file_prefix = sample_file_folder + data_index
        solve_file_prefix = solve_file_folder + data_index
        recon_file_prefix = recon_file_folder + data_index

        solve_base_file_prefix = sample_file_prefix
        solve_sample_file_prefix = sample_file_prefix

        if not os.path.exists(sample_file_folder):
            os.makedirs(f"{sample_file_folder}")
            shutil.copyfile(
                in_filename, join(sample_file_folder, basename(in_filename))
            )
            # os.system(f'cp {in_filename} {sample_file_folder}')
        if not os.path.exists(solve_file_folder):
            os.makedirs(f"{solve_file_folder}")
        if not os.path.exists(recon_file_folder):
            os.makedirs(f"{recon_file_folder}")

        # build octree
        TIME_START_OCTREE = time()
        build_octree_cmd = f"{EXPORT_QUERY_EXE} -i {input} -o {sample_file_prefix} -d {max_depth} -m {min_depth} "
        print(f"\n[EXECUTING] {build_octree_cmd}\n")
        os.system(build_octree_cmd)
        TIME_END_OCTREE = time()

        # solve equation
        TIME_START_SOLVE = time()
        print("[INFO][Reconstructor::reconstructSurface]")
        print("\t start solve equation...")
        self.solver.solve(
            solve_base_file_prefix + "_normalized.npy",
            solve_sample_file_prefix + "_normalized.npy",
            sample_file_prefix + "_for_query.npy",
            solve_file_prefix + PARAM_MIDFIX,
            width_k,
            width_max,
            width_min,
            alpha,
            max_iters,
            save_r,
        )
        TIME_END_SOLVE = time()

        # reconstruction
        TIME_START_RECON = time()
        with open(f"{solve_file_prefix}{PARAM_MIDFIX}isoval.txt", "r") as isoval_file:
            isoval = isoval_file.read()
            isoval = eval(isoval)

        recon_cmd = (
            f"{LOAD_QUERY_EXE} -i {in_filename} -d {max_depth} -m {min_depth} "
            + f"--grid_val {solve_file_prefix}{PARAM_MIDFIX}eval_grid.npy "
            + f"--grid_width {solve_file_prefix}{PARAM_MIDFIX}grid_width.npy "
            + f"--isov {isoval} "
            + f"-o {recon_file_prefix}{PARAM_MIDFIX}recon.ply"
        )
        print(f"\n[EXECUTING] {recon_cmd}\n")
        os.system(recon_cmd)
        TIME_END_RECON = time()

        print(
            "\033[94m"
            + "[Timer] Note: Some preprocessing (width computation) is actually in the Main part."
            + "\033[0m"
        )
        print(
            "\033[94m"
            + f"[Timer] Pre:    {TIME_END_OCTREE-TIME_START_OCTREE}"
            + "\033[0m"
        )
        print(
            "\033[94m"
            + f"[Timer] Main:   {TIME_END_SOLVE-TIME_START_SOLVE}"
            + "\033[0m"
        )
        print(
            "\033[94m"
            + f"[Timer] Post:   {TIME_END_RECON-TIME_START_RECON}"
            + "\033[0m"
        )
        print(
            "\033[94m"
            + f"[Timer] Total:  {TIME_END_OCTREE-TIME_START_OCTREE+TIME_END_SOLVE-TIME_START_SOLVE+TIME_END_RECON-TIME_START_RECON}"
            + "\033[0m"
        )
        return True
