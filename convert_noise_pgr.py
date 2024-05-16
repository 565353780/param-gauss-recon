import os
from shutil import copyfile

def demo(gauss_sigma: float = 0.01):
    print('start convert new data...')

    high_reso = False
    if high_reso:
        sample = '20000'
        alpha = '1.05'
        width_k = '7'
        width_min = '0.0015' # 0.0015
        width_max = '0.015' # 0.015
    else:
        sample = '4000'
        alpha = '1.2'
        width_k = '1'
        width_min = '0.0015' # 0.0015
        width_max = '0.015' # 0.015

    noise_label = 'Noise_' + str(gauss_sigma).replace('.', '-')

    first_solve_list = ['03001627']
    for category_id in first_solve_list:
        dataset_folder_path = '/home/chli/chLi/Dataset/SampledPcd_Manifold_' + noise_label + '/ShapeNet/' + category_id + '/'
        result_folder_path = './output/recon/sample_' + sample + '_k_' + width_k + '_min_' + width_min + '_max_' + width_max + '_alpha_' + alpha + '_depth_min_1_depth_max_1/'
        save_folder_path = '/home/chli/chLi/Dataset/PGR_Manifold_' + noise_label + '_Recon_' + sample + '/ShapeNet/' + category_id + '/'
        os.makedirs(save_folder_path, exist_ok=True)

        solved_shape_names = os.listdir(save_folder_path)

        pcd_filename_list = os.listdir(dataset_folder_path)
        pcd_filename_list.sort()

        for i, pcd_filename in enumerate(pcd_filename_list):
            if pcd_filename[-4:] != '.npy':
                continue

            if pcd_filename.replace('.npy', '.ply') in solved_shape_names:
                continue

            pcd_file_path = dataset_folder_path + pcd_filename

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

            recon_mesh_file_path = result_folder_path + pcd_filename.split('.npy')[0] + '_sample-' + sample + '_recon_pgr.ply'
            if os.path.exists(recon_mesh_file_path):
                save_mesh_file_path = save_folder_path + pcd_filename.replace('.npy', '.ply')

                copyfile(recon_mesh_file_path, save_mesh_file_path)

            print('category:', category_id, 'solved shape num:', i + 1)

    print('convert new data finished!')
    return True

if __name__ == "__main__":
    demo(0.002)
    demo(0.005)
    demo(0.01)
