import numpy as np
import open3d as o3d
from math import ceil
from typing import Union


def getPointCloud(pts: np.ndarray):
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(pts)
    return pcd


def downSample(pcd: o3d.geometry.PointCloud, sample_point_num: int, voxel_size: Union[float , None]=None) -> o3d.geometry.PointCloud:
    if sample_point_num < 1:
        print("[WARN][pcd::downSample]")
        print("\t sample point num < 1! will use source pcd!")
        print("\t sample_point_num:", sample_point_num)
        return pcd

    if voxel_size is not None:
        down_sample_pcd = pcd.voxel_down_sample(voxel_size)
        # print(down_sample_pcd)
        #o3d.visualization.draw_geometries([down_sample_pcd])
        #exit()
        return down_sample_pcd

    try:
        down_sample_pcd = pcd.farthest_point_down_sample(sample_point_num)
        return down_sample_pcd
    except:
        every_k_points = ceil(np.asarray(pcd.points).shape[0] / sample_point_num)
        down_sample_pcd = pcd.uniform_down_sample(every_k_points)
        return down_sample_pcd
