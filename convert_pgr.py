import os
from time import sleep
from shutil import copyfile

def demo():
    print('start convert new data...')

    sample = '4000'
    alpha = '1.2'
    width_k = '1'

    first_solve_list = ['03001627', '02691156']
    first_solve_list = ['03001627']
    for category_id in first_solve_list:
        compare_folder_path = '../ma-sh/output/metric_manifold_result_selected/ShapeNet/' + category_id + '/'

        first_solve_shape_ids = None
        if os.path.exists(compare_folder_path):
            first_solve_shape_ids = os.listdir(compare_folder_path)
        first_solve_shape_ids = None

        dataset_folder_path = '/home/chli/chLi/Dataset/SampledPcd_Manifold/ShapeNet/' + category_id + '/'
        result_folder_path = './output/recon/sample_' + sample + '_k_' + width_k + '_min_0.0015_max_0.015_alpha_' + alpha + '_depth_min_1_depth_max_1/'
        save_folder_path = '/home/chli/chLi/Dataset/PGR_Manifold_Recon_' + sample + '/ShapeNet/' + category_id + '/'
        os.makedirs(save_folder_path, exist_ok=True)

        solved_shape_names = os.listdir(save_folder_path)

        pcd_filename_list = os.listdir(dataset_folder_path)
        pcd_filename_list.sort()

        for i, pcd_filename in enumerate(pcd_filename_list):
            if pcd_filename[-4:] != '.npy':
                continue

            if first_solve_shape_ids is not None:
                if pcd_filename.split('.npy')[0] not in first_solve_shape_ids:
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
    while True:
        demo()
        sleep(10)
