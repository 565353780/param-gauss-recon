import torch
from typing import Union
from tqdm import tqdm
from scipy.spatial import KDTree

from param_gauss_recon.Data.pgr_params import PGRParams
from param_gauss_recon.Method.kernel import get_A


def get_query_vals(queries: torch.Tensor, q_width: torch.Tensor,
                   y_base: torch.Tensor, lse: torch.Tensor, chunk_size: int) -> torch.Tensor:
    ### getting values
    split_length = queries.shape[0] // chunk_size + 1
    split_num = queries.shape[0] // split_length
    query_chunks = torch.split(queries, split_num)
    q_cut_chunks = torch.split(q_width, split_num)
    query_vals = []

    print("[INFO][utils::solve]")
    print('\t start query on the grid...')
    for chunk, cut_chunk in tqdm(list(zip(query_chunks, q_cut_chunks))):
        A_show = get_A(chunk, y_base, cut_chunk)
        query_vals.append(torch.matmul(A_show, lse))

    query_vals = torch.cat(query_vals, dim=0)
    return query_vals


def get_width(
    query_set: torch.Tensor,
    pgr_params: PGRParams,
    base_set: Union[torch.Tensor, None] = None,
    base_kdtree: Union[KDTree, None] = None,
):
    """
    query_set: [Nx, 3], Nx points of which the widths are needed

    k: int, k for kNN to query the tree

    base_set: [Ny, 3], Ny points to compute the widths

    base_kdtree: this function will build a kdtree for `base_set`.
        However, if you have built one before this call, just pass it here.

    returns:
    (query_widths: [Nx], kdtree_base_set)
    """

    if pgr_params.width_min > pgr_params.width_max:
        x_width = torch.ones(query_set.shape[0], dtype=query_set.dtype) * pgr_params.width_min

        return x_width, None

    if base_kdtree is None:
        assert base_set is not None
        base_kdtree = KDTree(base_set.cpu().numpy())

    x_knn_dist, _ = base_kdtree.query(query_set.cpu().numpy(), k=pgr_params.width_k + 2)  # [N, k],
    x_knn_dist = torch.from_numpy(x_knn_dist).type(query_set.dtype).to(query_set.device)

    x_width = torch.sqrt(torch.einsum("nk,nk->n", x_knn_dist[:, 1:], x_knn_dist[:, 1:]) / pgr_params.width_k)
    torch.clip(x_width, pgr_params.width_min, pgr_params.width_max)

    return x_width, base_kdtree
