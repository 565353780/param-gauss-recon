import os

folder_path = "./output/save_itr_27/"

pcd_filename_list = os.listdir(folder_path)
pcd_filename_list.sort()

for pcd_filename in pcd_filename_list:
    pcd_file_path = folder_path + pcd_filename

    cmd = (
        "python run_pgr.py "
        + pcd_file_path
        + " --sample 2000"
        + " --alpha 8.0"
        + " --width_k 80"
    )

    print("start run:")
    print(cmd)
    os.system(cmd)
    exit()
