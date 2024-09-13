import math
import torch
import numpy as np
from typing import Union
from tqdm import tqdm, trange
from scipy.spatial import KDTree

from param_gauss_recon.Method.kernel import mul_A_T, get_A, get_B


def solve(
    x: torch.Tensor, y: torch.Tensor, x_width: torch.Tensor,
    chunk_size: int, iso_value: float, r_sq_stop_eps: float,
    alpha: float, max_iters: Union[int, None], save_r: Union[str, None]):
    """
    x: numpy array of shape [N_query, 3]
    y: numpy array of shape [N_sample, 3]
    ---
    return:
    lse:
    """
    N_query = x.shape[0]

    if max_iters is None:
        max_iters = y.shape[0]
    else:
        max_iters = min(max_iters, y.shape[0])

    print("[INFO][utils::solve]")
    print('\t start pre-computing B...')
    B = get_B(x, y, chunk_size, x_width, alpha)

    xi = torch.zeros(N_query, dtype=x.dtype)
    r = torch.ones(N_query, dtype=x.dtype) * iso_value
    p = r.clone()

    if save_r:
        r_list = []

    print("[INFO][utils::solve]")
    print('\t start CG iterations...')
    solve_progress_bar = trange(max_iters)
    k = -1  # to prevent error message when max_iters == 0
    for k in solve_progress_bar:
        Bp = torch.matmul(B, p)
        r_sq = r.dot(r)

        alpha = r_sq / p.dot(Bp)
        xi += alpha * p
        r -= alpha * Bp
        beta = r.dot(r) / r_sq
        p *= beta
        p += r

        solve_progress_bar.set_description(
            f"[In solver] {r_sq.item():.2e}/{r_sq_stop_eps:.0e}"
        )

        if save_r:
            r_list.append(math.sqrt(r_sq.item()))

        if r_sq < r_sq_stop_eps:
            solve_progress_bar.close()
            break
    lse = mul_A_T(x, y, xi, x_width, chunk_size)

    if save_r:
        return lse, r_list
    return lse

def get_query_vals(queries: torch.Tensor, q_width: torch.Tensor,
                   y_base: torch.Tensor, lse: torch.Tensor, chunk_size: int):
    ### getting values
    split_length = queries.shape[0] // chunk_size + 1
    split_num = queries.shape[0] // split_length
    query_chunks = torch.split(queries, split_num)
    q_cut_chunks = torch.split(q_width, split_num)
    query_vals = []

    print("[INFO][utils::solve]")
    print('\t start query on the grid...')
    tqdmbar_query = tqdm(list(zip(query_chunks, q_cut_chunks)))

    for chunk, cut_chunk in tqdmbar_query:
        A_show = get_A(chunk, y_base, cut_chunk)
        query_vals.append(torch.matmul(A_show, lse))

    query_vals = np.concatenate(query_vals, axis=0)
    return query_vals


def get_width(
    query_set: torch.Tensor,
    k: int,
    width_min: float,
    width_max: float,
    base_set: Union[torch.Tensor, None] = None,
    base_kdtree: Union[KDTree, None] = None,
    return_kdtree: bool=False,
):
    """
    query_set: [Nx, 3], Nx points of which the widths are needed

    k: int, k for kNN to query the tree

    base_set: [Ny, 3], Ny points to compute the widths

    base_kdtree: this function will build a kdtree for `base_set`.
        However, if you have built one before this call, just pass it here.

    return_kdtree: whether to return the built kdtree for future use.

    ---------------
    Note: one of `base_set` and `base_kdtree` must be given
    ---------------

    returns:

    if return_kdtree == False, then returns:
        query_widths: [Nx]
    if return_kdtree == True, then returns a tuple:
        (query_widths: [Nx], kdtree_base_set)
    """

    if base_kdtree is None:
        assert base_set is not None
        base_kdtree = KDTree(base_set)

    x_knn_dist, x_knn_idx = base_kdtree.query(query_set, k=k + 2)  # [N, k],
    x_knn_dist = torch.from_numpy(x_knn_dist).type(query_set.dtype)

    x_width = torch.sqrt(torch.einsum("nk,nk->n", x_knn_dist[:, 1:], x_knn_dist[:, 1:]) / k)
    x_width[x_width > width_max] = width_max
    x_width[x_width < width_min] = width_min

    if return_kdtree:
        return x_width, base_kdtree
    else:
        del base_kdtree
        return x_width
