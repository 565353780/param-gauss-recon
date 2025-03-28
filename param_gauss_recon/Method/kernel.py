import torch
from tqdm import trange

from param_gauss_recon.Config.constant import EPSILON
from param_gauss_recon.Data.pgr_params import PGRParams


def get_A(x: torch.Tensor, y: torch.Tensor, x_width: torch.Tensor):
    """
    x: numpy array of shape [N_query, 3]
    y: numpy array of shape [N_sample, 3]
    x_width: [N_query]
    ---
    return:
    A: numpy array of shape [N_query, 3 * N_sample]
    """

    N_query = x.shape[0]

    A = x[:, None] - y[None, :]  # [N_query, N_sample, 3]
    dist = torch.sqrt((A**2).sum(-1))  # [N_query, N_sample], |x^i-y^j|^2
    dist[dist == 0.0] += EPSILON

    # inv_dist = cnp.where(dist > x_width[:, None], 1/dist, 0.) # / 4 / cp.pi
    inv_dist = torch.where(
        dist > x_width[:, None], 1 / dist, 1 / x_width[:, None]
    )  # / 4 / cp.pi
    inv_cub_dist = inv_dist**3 / 4 / torch.pi  # [N_query, N_sample]

    A *= inv_cub_dist[..., None]  # [N_query, N_sample, 3]
    A = A.permute(0, 2, 1)
    A = A.reshape(N_query, -1)

    return -A

def mul_A_T(x: torch.Tensor, y: torch.Tensor, xi: torch.Tensor,
            x_width: torch.Tensor, chunk_size: int):
    N_sample = y.shape[0]
    n_y_chunks = N_sample // chunk_size + 1

    lse = torch.zeros((3, N_sample), dtype=x.dtype, device=x.device)
    for i in range(n_y_chunks):
        y_chunk = y[i * chunk_size : (i + 1) * chunk_size]
        A_chunk = get_A(x, y_chunk, x_width)
        (
            lse[0, i * chunk_size : (i + 1) * chunk_size],
            lse[1, i * chunk_size : (i + 1) * chunk_size],
            lse[2, i * chunk_size : (i + 1) * chunk_size],
        ) = torch.einsum("jk,j", A_chunk, xi).reshape(3, -1)

    return lse.reshape(-1)

def get_B(x: torch.Tensor, y: torch.Tensor,
          chunk_size: int, x_width: torch.Tensor, alpha: float):
    """
    x: numpy array of shape [N_query, 3]
    y: numpy array of shape [N_sample, 3]
    ---
    return:
    B: numpy array of shape [N_query, N_query], which is AA^T
    """
    N_query = x.shape[0]

    B = torch.zeros([N_query, N_query], dtype=x.dtype, device=x.device)

    n_row_chunks = N_query // chunk_size + 1
    n_col_chunks = n_row_chunks

    for i in trange(n_row_chunks):
        x_chunk_i = x[i * chunk_size : (i + 1) * chunk_size]

        if x_chunk_i.shape[0] <= 0:
            continue

        x_chunk_eps_i = x_width[i * chunk_size : (i + 1) * chunk_size]
        A_block_i = get_A(x_chunk_i, y, x_chunk_eps_i)

        for j in range(i, n_col_chunks):
            x_chunk_j = x[j * chunk_size : (j + 1) * chunk_size]

            if x_chunk_j.shape[0] <= 0:
                continue

            x_chunk_eps_j = x_width[j * chunk_size : (j + 1) * chunk_size]
            A_block_j = get_A(x_chunk_j, y, x_chunk_eps_j)

            B[
                i * chunk_size : (i + 1) * chunk_size,
                j * chunk_size : (j + 1) * chunk_size,
            ] = torch.einsum("ik,jk->ij", A_block_i, A_block_j)

            if i != j:
                B[
                    j * chunk_size : (j + 1) * chunk_size,
                    i * chunk_size : (i + 1) * chunk_size,
                ] = B[
                    i * chunk_size : (i + 1) * chunk_size,
                    j * chunk_size : (j + 1) * chunk_size,
                ].T
            else:
                block_size = B[
                    i * chunk_size : (i + 1) * chunk_size,
                    j * chunk_size : (j + 1) * chunk_size,
                ].shape[0]
                if block_size <= 0:
                    continue
                diag_mask = torch.eye(block_size, dtype=torch.bool, device=x.device)
                # diag_sqr = (B[i*chunk_size:(i+1)*chunk_size, j*chunk_size:(j+1)*chunk_size][diag_mask])
                B[
                    i * chunk_size : (i + 1) * chunk_size,
                    j * chunk_size : (j + 1) * chunk_size,
                ][diag_mask] *= alpha

    return B

def solveLSE(
    x: torch.Tensor, y: torch.Tensor, x_width: torch.Tensor,
    chunk_size: int, iso_value: float, r_sq_stop_eps: float,
    pgr_params: PGRParams):
    """
    x: numpy array of shape [N_query, 3]
    y: numpy array of shape [N_sample, 3]
    ---
    return:
    lse:
    """
    print("[INFO][kernel::solveLSE]")
    print('\t start pre-computing B...')
    B = get_B(x, y, chunk_size, x_width, pgr_params.alpha)

    xi = torch.zeros(x.shape[0], dtype=x.dtype, device=x.device)
    r = torch.ones(x.shape[0], dtype=x.dtype, device=x.device) * iso_value
    p = r.clone()

    print("[INFO][kernel::solveLSE]")
    print('\t start CG iterations...')
    solve_progress_bar = trange(y.shape[0])
    for _ in solve_progress_bar:
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

        if r_sq < r_sq_stop_eps:
            solve_progress_bar.close()
            break
    lse = mul_A_T(x, y, xi, x_width, chunk_size)

    return lse
