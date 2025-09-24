import os
import open3d as o3d
from typing import Union
from shutil import copyfile
from os.path import join, basename

from param_gauss_recon.Config.path import EXPORT_QUERY_EXE, LOAD_QUERY_EXE
from param_gauss_recon.Data.pgr_params import PGRParams
from param_gauss_recon.Method.path import createFileFolder
from param_gauss_recon.Module.pcd_sampler import PcdSampler
from param_gauss_recon.Module.solver import Solver


class Reconstructor(object):
    def __init__(self) -> None:
        self.pcd_sampler = PcdSampler()
        self.solver = Solver()
        return

    def toSampledPcdFile(self, input: str, sample_point_num: Union[int, None]) -> str:
        if sample_point_num is not None:
            if sample_point_num > 0:
                pcd_file_name = input.split("/")[-1]

                pcd_file_type = "." + pcd_file_name.split(".")[-1]

                save_pcd_file_path = "./output/sample_pcd/" + pcd_file_name.replace(
                    pcd_file_type, "_sample-" + str(sample_point_num) + ".xyz"
                )

                print("[INFO][Reconstructor::toSampledPcdFile]")
                print("\t start toFPSPcdFile...")
                if not self.pcd_sampler.toFPSPcdFile(
                    input, sample_point_num, save_pcd_file_path
                ):
                    print("[WARN][Reconstructor::toSampledPcdFile]")
                    print("\t toFPSPcdFile failed!")
                    print("\t try to start reconstruct with the input point cloud...")
                    return input

                return save_pcd_file_path

        if ".xyz" in input:
            return input

        pcd_file_name = input.split("/")[-1]
        pcd_file_type = "." + pcd_file_name.split(".")[-1]

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
        pgr_params: PGRParams,
        tmp_folder_path: str = "./output/",
        save_mesh_file_path: Union[str, None] = None,
    ) -> bool:
        save_pcd_file_path = self.toSampledPcdFile(input, pgr_params.sample_point_num)

        self.param_midfix = pgr_params.toLogStr()

        pgr_params.outputInfo()

        in_filename = save_pcd_file_path
        data_index = in_filename.split("/")[-1][:-4]

        results_folder = tmp_folder_path
        sample_file_folder = results_folder + data_index + "/samples/"
        solve_file_folder = results_folder + data_index + "/solve/"
        recon_file_folder = results_folder + data_index + "/recon/"

        sample_file_prefix = sample_file_folder + data_index
        solve_file_prefix = solve_file_folder + data_index
        recon_file_prefix = recon_file_folder + data_index

        if not os.path.exists(sample_file_folder):
            os.makedirs(f"{sample_file_folder}")
            copyfile(in_filename, join(sample_file_folder, basename(in_filename)))
        os.makedirs(solve_file_folder, exist_ok=True)
        os.makedirs(recon_file_folder, exist_ok=True)

        # build octree
        build_octree_cmd = (
            f"{EXPORT_QUERY_EXE} -i {save_pcd_file_path} -o {sample_file_prefix}"
            + pgr_params.toCMDStr()
        )
        print("[INFO][Reconstructor::reconstructSurface]")
        print("\t [EXECUTING]", build_octree_cmd)
        os.system(build_octree_cmd)

        # solve equation
        print("[INFO][Reconstructor::reconstructSurface]")
        print("\t start solve equation...")
        self.solver.solve(
            sample_file_prefix + "_normalized.npy",
            sample_file_prefix + "_for_query.npy",
            solve_file_prefix + self.param_midfix,
            pgr_params,
        )

        if not pgr_params.recon_mesh:
            return True

        # reconstruction
        with open(
            f"{solve_file_prefix}{self.param_midfix}_isoval.txt", "r"
        ) as isoval_file:
            isoval = isoval_file.read()
            isoval = eval(isoval)

        recon_cmd = (
            f"{LOAD_QUERY_EXE} -i {in_filename}"
            + pgr_params.toCMDStr()
            + f" --grid_val {solve_file_prefix}{self.param_midfix}_eval_grid.npy"
            + f" --grid_width {solve_file_prefix}{self.param_midfix}_grid_width.npy"
            + f" --isov {isoval}"
            + f" -o {recon_file_prefix}{self.param_midfix}_recon.ply"
        )
        print("[INFO][Reconstructor::reconstructSurface]")
        print("\t [EXECUTING]", recon_cmd)
        os.system(recon_cmd)

        if save_mesh_file_path is not None:
            createFileFolder(save_mesh_file_path)

            copyfile(
                "./" + recon_file_prefix + self.param_midfix + "_recon.ply",
                save_mesh_file_path,
            )
        return True
