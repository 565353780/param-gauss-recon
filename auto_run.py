import os

folder_path = "/home/chli/chLi/Dataset/SampledPcd/ShapeNet/03001627/"

pcd_filename_list = os.listdir(folder_path)
pcd_filename_list.sort()

start_idx = 0
result_folder_path = './output/recon/123/'
if os.path.exists(result_folder_path):
    start_idx = len(os.listdir(result_folder_path))

for i, pcd_filename in enumerate(pcd_filename_list):
    if i < start_idx:
        continue

    pcd_file_path = folder_path + pcd_filename
    #pcd_file_path = '/home/chli/github/ASDF/ma-sh/output/normal_mash/103a0a413d4c3353a723872ad91e4ed1.ply'
    #pcd_file_path = '../ma-sh/output/normal_mash/gs_30000itr.ply'

    cmd = (
        "python run_pgr.py "
        + pcd_file_path
        + " --sample 2934"
        + " --alpha 1.05"
        + " --width_k 7"
    )

    print("start run shape[" + str(i) + "]:")
    print(cmd)
    os.system(cmd)
    exit()
