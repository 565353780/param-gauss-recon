import numpy as np
import open3d as o3d
from math import ceil


def getPointCloud(pts: np.ndarray):
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(pts)
    return pcd


def downSample(pcd, sample_point_num):
    if sample_point_num < 1:
        print("[WARN][pcd::downSample]")
        print("\t sample point num < 1!")
        print("\t sample_point_num:", sample_point_num)
        return None

    if False:
        down_sample_pcd = pcd.voxel_down_sample(0.015)
        #o3d.visualization.draw_geometries([down_sample_pcd])
        #exit()
        return down_sample_pcd

    try:
        down_sample_pcd = pcd.farthest_point_down_sample(sample_point_num)
    except:
        every_k_points = ceil(np.asarray(pcd.points).shape[0] / sample_point_num)
        down_sample_pcd = pcd.uniform_down_sample(every_k_points)

    return down_sample_pcd
