import os

pcd_file_path = "/home/chli/Downloads/chair-denoise2.ply"
pcd_file_path = "../ma-sh/output/hotdog_final.ply"
pcd_file_path = "../mash-gs/output/NeRF_hotdog_train_20240520_10:29:10/point_cloud/iteration_4000/point_cloud.ply"
pcd_file_path = "../ma-sh/output/20240520_10:49:21/pcd/535_final.ply"
pcd_file_path = "../ma-sh/output/20240520_11:35:57/pcd/619_final.ply"
pcd_file_path = "../mvs-former/output/ship_train/ship_train.ply"
pcd_file_path = "../mvs-former/output/hotdog_train/hotdog_train.ply"
pcd_file_path = "../gaussian-splatting/output/NeRF_mic_train_20240515_17:49:58/point_cloud/iteration_22500/point_cloud.ply"

pcd_file_path = "/Users/fufu/Downloads/Dataset/DTU/dtu_37_max/archive/2dgs-pcd.ply"

sample = '20000'
alpha = '1.05' # 1.05
width_k = '7' # 7
width_min = '0.0015' # 0.0015
width_max = '0.015' # 0.015

cmd = (
    "python run_pgr.py "
    + pcd_file_path
    + " --sample " + sample
    + " --alpha " + alpha
    + " --width_k " + width_k
    + " --width_min " + width_min
    + " --width_max " + width_max
    + " --cpu"
)

print(cmd)

os.system(cmd)
