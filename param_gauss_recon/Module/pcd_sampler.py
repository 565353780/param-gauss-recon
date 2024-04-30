import os
import numpy as np
import open3d as o3d
from typing import Union

from param_gauss_recon.Method.path import createFileFolder
from param_gauss_recon.Method.pcd import downSample, getPointCloud


class PcdSampler(object):
    def __init__(self) -> None:
        return

    def toFPSPcd(
        self, pcd: o3d.geometry.PointCloud, sample_point_num: int
    ) -> Union[o3d.geometry.PointCloud, None]:
        if np.asarray(pcd.points).shape[0] < sample_point_num:
            return pcd

        down_sample_pcd = downSample(pcd, sample_point_num)

        if down_sample_pcd is None:
            print("[ERROR][PcdSampler::toFPSPcd]")
            print("\t downSample failed!")
            return None

        return down_sample_pcd

    def toFPSPoints(
        self, points: np.ndarray, sample_point_num: int
    ) -> Union[np.ndarray, None]:
        if points.shape[0] < sample_point_num:
            return points

        pcd = getPointCloud(points)

        down_sample_pcd = self.toFPSPcd(pcd, sample_point_num)

        if down_sample_pcd is None:
            print("[ERROR][PcdSampler::toFPSPoints]")
            print("\t downSample failed!")
            return None

        return down_sample_pcd

    def toFPSPcdFile(
        self, pcd_file_path: str, sample_point_num: int, save_pcd_file_path: str
    ) -> bool:
        if not os.path.exists(pcd_file_path):
            print("[ERROR][PcdSampler::toFPSPcdFile]")
            print("\t pcd file not exist!")
            print("\t pcd_file_path:", pcd_file_path)
            return False

        pcd = o3d.io.read_point_cloud(pcd_file_path)

        down_sample_pcd = self.toFPSPcd(pcd, sample_point_num)

        if down_sample_pcd is None:
            print("[ERROR][PcdSampler::toFPSPcdFile]")
            print("\t downSample failed!")
            return False

        createFileFolder(save_pcd_file_path)

        o3d.io.write_point_cloud(save_pcd_file_path, down_sample_pcd, write_ascii=True)
        return True
