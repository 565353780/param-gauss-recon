import os
from tqdm import tqdm
from time import sleep
from shutil import copyfile

def demo():
    print('start convert new data...')

    current_appendix = ''
    dataset_folder_path = '/home/chli/chLi/Dataset/MashPcd_Manifold' + current_appendix + '/ShapeNet/'
    save_folder_path = '/home/chli/chLi/Dataset/MashV4_Recon' + current_appendix + '/ShapeNet/'
    sample = '20000'
    alpha = '1.05' # 1.05
    width_k = '7' # 7
    width_min = '0.0015' # 0.0015
    width_max = '0.015' # 0.015

    result_folder_path = './output/recon/sample_' + sample + '_k_' + width_k + '_min_' + width_min + '_max_' + width_max + '_alpha_' + alpha + '_depth_min_1_depth_max_1/'

    classname_list = os.listdir(dataset_folder_path)
    classname_list.sort()

    model_filename_list_dict = {}
    solved_model_filename_list_dict = {}
    max_shape_num = 0

    for classname in tqdm(classname_list):
        class_folder_path = dataset_folder_path + classname + '/'

        model_filename_list = os.listdir(class_folder_path)
        model_filename_list.sort()

        max_shape_num = max(max_shape_num, len(model_filename_list))
        model_filename_list_dict[classname] = model_filename_list

        solved_model_filename_list_dict[classname] = []
        class_save_folder_path = save_folder_path + classname + "/"
        os.makedirs(class_save_folder_path, exist_ok=True)
        solved_model_filename_list = os.listdir(class_save_folder_path)
        solved_model_filename_list.sort()
        solved_model_filename_list_dict[classname] = solved_model_filename_list

    solved_shape_num = 0
    for i in range(max_shape_num):
        for classname, model_filename_list in model_filename_list_dict.items():
            if len(model_filename_list) <= i:
                continue

            model_filename = model_filename_list[i]

            if model_filename[-4:] != '.ply':
                continue

            solved_model_filename_list = solved_model_filename_list_dict[classname]
            if model_filename in solved_model_filename_list:
                continue

            class_folder_path = dataset_folder_path + classname + '/'
            class_save_folder_path = save_folder_path + classname + "/"

            pcd_file_path = class_folder_path + model_filename

            cmd = (
                "python run_pgr.py "
                + pcd_file_path
                + " --sample " + sample
                + " --alpha " + alpha
                + " --width_k " + width_k
                + " --width_min " + width_min
                + " --width_max " + width_max
            )

            print("start run shape[" + str(solved_shape_num) + "]:")
            print(cmd)

            os.system(cmd)

            recon_mesh_file_path = result_folder_path + model_filename.split('.ply')[0] + '_sample-' + sample + '_recon_pgr.ply'
            if os.path.exists(recon_mesh_file_path):
                save_mesh_file_path = class_save_folder_path + model_filename

                copyfile(recon_mesh_file_path, save_mesh_file_path)

                solved_shape_num += 1

            print('category:', classname, 'solved shape num:', solved_shape_num)

    print('convert new data finished!')
    return True

if __name__ == "__main__":
    while True:
        demo()
        sleep(10)
