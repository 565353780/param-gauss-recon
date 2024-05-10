import os

folder_path = "../mash-shape-diffusion/output/sample/save_itr_27-1/"

if True:
    sample = '20000'
    alpha = '1.08' # 1.05
    width_k = '7' # 7
    width_min = '0.0015' # 0.0015
    width_max = '0.015' # 0.015

if True:
    sample = '4000'
    alpha = '2.0' # 1.05
    width_k = '7' # 7
    width_min = '0.04' # 0.0015
    width_max = '0.04' # 0.015

pcd_filename_list = os.listdir(folder_path)
pcd_filename_list.sort()

for i, pcd_filename in enumerate(pcd_filename_list):
    if pcd_filename[-4:] != '.ply':
        continue

    pcd_file_path = folder_path + pcd_filename

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
