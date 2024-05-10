import os
from shutil import copyfile

current_appendix = ''
folder_path = "/home/chli/chLi/Dataset/MashPcd" + current_appendix + "/ShapeNet/03001627/"

if True:
    sample = '20000'
    alpha = '1.08' # 1.05
    width_k = '7' # 7
    width_min = '0.0015' # 0.0015
    width_max = '0.015' # 0.015

if False:
    sample = '10000'
    alpha = '2.0' # 1.05
    width_k = '7' # 7
    width_min = '0.04' # 0.0015
    width_max = '0.04' # 0.015

pcd_filename_list = os.listdir(folder_path)
pcd_filename_list.sort()

result_folder_path = './output/recon/sample_' + sample + '_k_' + width_k + '_min_' + width_min + '_max_' + width_max + '_alpha_' + alpha + '_depth_min_1_depth_max_1/'
save_folder_path = "/home/chli/chLi/Dataset/Mash_Recon" + current_appendix + "/ShapeNet/03001627/"
os.makedirs(save_folder_path, exist_ok=True)

solved_shape_names = os.listdir(save_folder_path)

for i, pcd_filename in enumerate(pcd_filename_list):
    if pcd_filename[-4:] != '.ply':
        continue

    if pcd_filename in solved_shape_names:
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

    recon_mesh_file_path = result_folder_path + pcd_filename.split('.ply')[0] + '_sample-' + sample + '_recon_pgr.ply'
    if os.path.exists(recon_mesh_file_path):
        save_mesh_file_path = save_folder_path + pcd_filename

        copyfile(recon_mesh_file_path, save_mesh_file_path)

    print('solved shape num:', i + 1)
