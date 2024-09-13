import torch


class PGRParams(object):
    def __init__(self) -> None:
        self.sample_point_num = 2000
        self.width_k = 7
        self.width_min = 0.0015
        self.width_max = 0.015
        self.alpha = 1.05
        self.max_iters = None # int
        self.min_depth = 1
        self.max_depth = 10
        self.dtype = torch.float32
        self.device = 'cuda'
        self.save_r = None # str
        self.recon_mesh = True

        if not torch.cuda.is_available():
            print('[WARN][PGRParams::__init__]')
            print('\t torch cuda is not available! will use cpu!')
            self.device = 'cpu'
        return

    def toCMDStr(self) -> str:
        cmd_str = ""

        cmd_str += " -d " + str(self.max_depth)
        cmd_str += " -m " + str(self.min_depth)

        return cmd_str

    def toLogStr(self) -> str:
        log_str = ""

        if self.sample_point_num is not None:
            log_str += "_sample_" + str(self.sample_point_num)

        log_str += "_k_" + str(self.width_k)
        log_str += "_min_" + str(self.width_min)
        log_str += "_max_" + str(self.width_max)
        log_str += "_alpha_" + str(self.alpha)
        log_str += "_depth_min_" + str(self.min_depth)
        log_str += "_depth_max_" + str(self.min_depth)

        return log_str

    def outputInfo(self, info_level: int = 0) -> bool:
        line_start = ""
        for _ in range(info_level):
            line_start += "\t"

        info_str = ""
        info_str += line_start + "---------Settings---------\n"
        info_str += line_start + f"min depth:   {self.min_depth}\n"
        info_str += line_start + f"max depth:   {self.max_depth}\n"
        info_str += line_start + f"max iters:   {self.max_iters}\n"
        info_str += line_start + f"width_k:     {self.width_k}\n"
        info_str += line_start + f"width_min:   {self.width_min}\n"
        info_str += line_start + f"width_max:   {self.width_max}\n"
        info_str += line_start + f"alpha:       {self.alpha}\n"
        info_str += line_start + "---------Settings---------"

        print(info_str)

        return True
