import os

pcd_file_path = "../ma-sh/output/metric_manifold_result_selected/ShapeNet/03001627/1e2ddaef401676915a7934ad3293bab5/mash_pcd.ply"

if True:
    sample = '40000'
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
)

print(cmd)

os.system(cmd)
