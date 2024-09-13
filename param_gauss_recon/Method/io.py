import os
import torch
import numpy as np
from typing import Union


def loadNpyData(file_path: str, dtype = torch.float32, device = 'cpu') -> Union[torch.Tensor, None]:
    if not os.path.exists(file_path):
        print('[ERROR][io::loadNpyData]')
        print('\t file not exist!')
        print('\t file_path :', file_path)
        return None

    data = np.load(file_path)
    data = torch.from_numpy(data).type(dtype).to(device)
    return data
