# Param Gauss Recon

## Source

```bash
https://github.com/jsnln/ParametricGaussRecon
```

## Download

```bash
https://github.com/isl-org/Open3D/releases/download/v0.19.0/open3d-devel-linux-x86_64-pre-cxx11-abi-0.19.0.tar.xz
->./3rd/open3d-devel-linux/
https://github.com/isl-org/Open3D/releases/download/v0.19.0/open3d-devel-windows-amd64-0.19.0.zip
->./3rd/open3d-devel-win/
```

## Setup

```bash
conda create -n pgr python=3.10
conda activate pgr
./setup.sh
./compile_pgr.sh # or .bat for windows
./compile_simple.sh # or .bat for windows
```

## Run

```bash
python run_pgr.py <your-pointcloud-file-path>
```

## Notes

```bash
smooth:
  width_k upper(e.g. 16)
  alpha upper(e.g. 1.2)
sparse input:
  alpha upper(e.g. 2.0)
  width_min upper(e.g. 0.04)
```

## Enjoy it~
