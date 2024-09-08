import torch

CHUNK_SIZE = 512
FLT_TYPE = torch.float32
R_SQ_STOP_EPS = 1e-20
TARGET_ISO_VALUE = -0.5  # this is compatible with the MC orientation
EPSILON = 1e-6
