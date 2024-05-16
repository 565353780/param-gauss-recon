import os

nerf_list = ['chair', 'hotdog', 'mic', 'ship']
mode_list = ['mash', 'gs']

nerf = nerf_list[1]
mode = mode_list[0]

if nerf == 'chair':
    if mode == 'mash':
        pcd_file_path = "../mash-gs/output/NeRF_chair_train_20240515_04:01:47/point_cloud/iteration_8000/point_cloud.ply"
        # voxel downsample = 0.015
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.005' # 0.015
    elif mode == 'gs':
        pcd_file_path = "../gaussian-splatting/output/NeRF_chair_train_20240515_16:44:15/point_cloud/iteration_27500/point_cloud.ply"
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.005' # 0.015
elif nerf == 'hotdog':
    if mode == 'mash':
        pcd_file_path = "../mash-gs/output/NeRF_hotdog_train_20240515_20:57:26/point_cloud/iteration_12500/point_cloud.ply"
        # voxel downsample = 0.02
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.02' # 0.015
    elif mode == 'gs':
        pcd_file_path = "../gaussian-splatting/output/NeRF_hotdog_train_20240515_17:18:42/point_cloud/iteration_110000/point_cloud.ply"
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.005' # 0.015
elif nerf == 'mic':
    if mode == 'mash':
        pcd_file_path = "../mash-gs/output/NeRF_mic_train_20240515_15:20:00/point_cloud/iteration_14500/point_cloud.ply"
        # voxel downsample = 0.015
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.02' # 0.015
    elif mode == 'gs':
        pcd_file_path = "../gaussian-splatting/output/NeRF_mic_train_20240515_17:49:58/point_cloud/iteration_22500/point_cloud.ply"
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.005' # 0.015
elif nerf == 'ship':
    if mode == 'mash':
        pcd_file_path = "../mash-gs/output/NeRF_ship_train_20240515_18:03:18/point_cloud/iteration_1000/point_cloud.ply"
        # voxel downsample = 0.015
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.02' # 0.015
    elif mode == 'gs':
        pcd_file_path = "../gaussian-splatting/output/NeRF_ship_train_20240515_17:56:55/point_cloud/iteration_97500/point_cloud.ply"
        sample = '10000'
        alpha = '1.05' # 1.05
        width_k = '7' # 7
        width_min = '0.0015' # 0.0015
        width_max = '0.005' # 0.015



cmd = (
    "python run_pgr.py "
    + pcd_file_path
    + " --sample " + sample
    + " --alpha " + alpha
    + " --width_k " + width_k
    + " --width_min " + width_min
    + " --width_max " + width_max
)

print(cmd)

os.system(cmd)
