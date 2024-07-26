import os
import shutil
import open3d as o3d
from time import time
from typing import Union
from os.path import join, basename

from param_gauss_recon.Config.path import EXPORT_QUERY_EXE, LOAD_QUERY_EXE
from param_gauss_recon.Method.path import createFileFolder
from param_gauss_recon.Module.pcd_sampler import PcdSampler
from param_gauss_recon.Module.solver import Solver


class Reconstructor(object):
    def __init__(self) -> None:
        self.pcd_sampler = PcdSampler()
        self.solver = Solver()

        self.PARAM_MIDFIX = None
        return

    def toSampledPcdFile(self, input: str, sample_point_num: int) -> str:
        if sample_point_num is not None:
            if sample_point_num > 0:
                pcd_file_name = input.split("/")[-1]

                pcd_file_type = '.' + pcd_file_name.split('.')[-1]

                save_pcd_file_path = "./output/sample_pcd/" + pcd_file_name.replace(
                    pcd_file_type, "_sample-" + str(sample_point_num) + ".xyz"
                )

                print("[INFO][Reconstructor::reconstructSurface]")
                print("\t start toFPSPcdFile...")
                if not self.pcd_sampler.toFPSPcdFile(
                    input, sample_point_num, save_pcd_file_path
                ):
                    print("[WARN][Reconstructor::reconstructSurface]")
                    print("\t toFPSPcdFile failed!")
                    print("\t try to start reconstruct with the input point cloud...")
                    save_pcd_file_path = input
        else:
            save_pcd_file_path = input
            if '.xyz' not in input:
                pcd_file_name = input.split("/")[-1]

                pcd_file_type = '.' + pcd_file_name.split('.')[-1]

                save_pcd_file_path = "./output/sample_pcd/" + pcd_file_name.replace(
                    pcd_file_type, ".xyz"
                )

                pcd = o3d.io.read_point_cloud(input)

                createFileFolder(save_pcd_file_path)

                o3d.io.write_point_cloud(save_pcd_file_path, pcd, write_ascii=True)
        return save_pcd_file_path

    def reconstructSurface(
        self,
        input: str,
        sample_point_num: Union[int, None] = None,
        width_k: int = 7,
        width_max: float = 0.015,
        width_min: float = 0.0015,
        alpha: float = 1.05,
        max_iters: Union[int, None] = None,
        max_depth: int = 10,
        min_depth: int = 1,
        cpu: bool = False,
        save_r: Union[str, None] = None,
        recon_mesh: bool = True,
    ) -> bool:
        save_pcd_file_path = self.toSampledPcdFile(input, sample_point_num)

        PARAM_MIDFIX = f"_k_{width_k}_min_{width_min}_max_{width_max}_alpha_{alpha}_depth_min_{min_depth}_depth_max_{min_depth}_"
        if sample_point_num is not None:
            PARAM_MIDFIX = f"_sample_{sample_point_num}" + PARAM_MIDFIX

        # self.PARAM_MIDFIX is only used for outer call recon results path
        self.param_midfix = PARAM_MIDFIX

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

        in_filename = save_pcd_file_path
        data_index = in_filename.split("/")[-1][:-4]

        results_folder = "results/"
        sample_file_folder = results_folder + data_index + "/samples/"
        solve_file_folder = results_folder + data_index + "/solve/"
        recon_file_folder = results_folder + data_index + "/recon/"

        sample_file_prefix = sample_file_folder + data_index
        solve_file_prefix = solve_file_folder + data_index
        recon_file_prefix = recon_file_folder + data_index

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
        build_octree_cmd = f"{EXPORT_QUERY_EXE} -i {save_pcd_file_path} -o {sample_file_prefix} -d {max_depth} -m {min_depth} "
        print(f"\n[EXECUTING] {build_octree_cmd}\n")
        os.system(build_octree_cmd)
        TIME_END_OCTREE = time()

        # solve equation
        TIME_START_SOLVE = time()
        print("[INFO][Reconstructor::reconstructSurface]")
        print("\t start solve equation...")
        self.solver.solve(
            sample_file_prefix + "_normalized.npy",
            sample_file_prefix + "_for_query.npy",
            solve_file_prefix + PARAM_MIDFIX,
            width_k,
            width_max,
            width_min,
            alpha,
            max_iters,
            cpu,
            save_r,
            recon_mesh,
        )
        TIME_END_SOLVE = time()

        if not recon_mesh:
            return True

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

        # copy recon results to output folder for faster visualization
        if True:
            recon_file_basename = recon_file_prefix.split('/')[-1]
            save_recon_folder_path = './output/recon/' + PARAM_MIDFIX[1:-1] + '/'

            os.makedirs(save_recon_folder_path, exist_ok=True)

            shutil.copyfile("./" + recon_file_prefix + PARAM_MIDFIX + "recon.ply", save_recon_folder_path + recon_file_basename + "_recon_pgr.ply")

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
