import os
from time import sleep
from shutil import copyfile

def main():
    folder_path = "/home/chli/chLi/Dataset/SampledPcd_Manifold/ShapeNet/03001627/"
    sample = '4000'
    alpha = '1.05'
    width_k = '7'

    pcd_filename_list = os.listdir(folder_path)
    pcd_filename_list.sort()

    result_folder_path = './output/recon/sample_' + sample + '_k_' + width_k + '_min_0.0015_max_0.015_alpha_' + alpha + '_depth_min_1_depth_max_1/'
    save_folder_path = '/home/chli/chLi/Dataset/PGR_Manifold_Recon_' + sample + '/ShapeNet/03001627/'
    os.makedirs(save_folder_path, exist_ok=True)

    solved_shape_names = os.listdir(save_folder_path)

    for i, pcd_filename in enumerate(pcd_filename_list):
        if pcd_filename[-4:] != '.npy':
            continue

        if pcd_filename.replace('.npy', '.ply') in solved_shape_names:
            continue

        pcd_file_path = folder_path + pcd_filename

        cmd = (
            "python run_pgr.py "
            + pcd_file_path
            + " --sample " + sample
            + " --alpha " + alpha
            + " --width_k " + width_k
        )

        print("start run shape[" + str(i) + "]:")
        print(cmd)

        os.system(cmd)

        recon_mesh_file_path = result_folder_path + pcd_filename.split('.npy')[0] + '_sample-' + sample + '_recon_pgr.ply'
        if os.path.exists(recon_mesh_file_path):
            save_mesh_file_path = save_folder_path + pcd_filename.replace('.npy', '.ply')

            copyfile(recon_mesh_file_path, save_mesh_file_path)

        print('solved shape num:', i + 1)

while True:
    main()
    sleep(10)
