import os

folder_path = "../mash-shape-diffusion/output/sample/save_itr_27-v2-1/"
folder_path = "/home/chli/chLi/Dataset/KITTI/trans_pcd/"

if True:
    sample = '20000'
    alpha = '1.05' # 1.05
    width_k = '7' # 7
    width_min = '0.0015' # 0.0015
    width_max = '0.015' # 0.015

if False:
    sample = '4000'
    alpha = '1.05' # 1.05
    width_k = '7' # 7
    width_min = '0.06' # 0.0015
    width_max = '0.06' # 0.015

i = 0
for root, _, files in os.walk(folder_path):
    for file in files:
        if not file.endswith('.ply'):
            continue

        pcd_file_path = root + '/' + file

        cmd = (
            "python run_pgr.py "
            + pcd_file_path
            + " --sample " + sample
            + " --alpha " + alpha
            + " --width_k " + width_k
            + " --width_min " + width_min
            + " --width_max " + width_max
        )

        print("start run shape[" + str(i) + "]:")
        print(cmd)

        os.system(cmd)

        print('solved shape num:', i + 1)
