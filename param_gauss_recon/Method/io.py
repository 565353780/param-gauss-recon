import os
import torch
import numpy as np
from typing import Union


def load_sample_from_npy(file_path: str, dtype) -> Union[torch.Tensor, None]:
    if not os.path.exists(file_path):
        print('[ERROR][utils::load_sample_from_npy]')
        print('\t file not exist!')
        print('\t file_path :', file_path)
        return None

    data = np.load(file_path)
    data = torch.from_numpy(data).type(dtype)
    return data
