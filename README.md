# Param Gauss Recon

## 快速部署

### 0. 系统准备

```bash
cmake
visual studio terminal # for win only
```

### 1. 下载conda环境包

```bash
# linux:
scp <user-id>@192.168.11.3:/mnt/sdb/data/chli/conda/conda-amcax.tar.gz ./3rd/conda-amcax.tar.gz
# win:
scp <user-id>@192.168.11.3:/mnt/sdb/data/chli/conda/conda-amcax-win.tar.gz ./3rd/conda-amcax-win.tar.gz
```

### 2. 恢复conda环境

```bash
./unpack.sh # or .bat for windows
```

### 3. 编译必要依赖

```bash
./compile_zlib.sh # or .bat for windows, use Developer PowerShell for VS
./compile_pgr.sh # or .bat for windows, use Developer PowerShell for VS
```

### 4. 编译可执行Demo

```bash
./compile_simple.sh # or .bat for windows, use Developer PowerShell for VS
```

### 5. 运行可执行Demo

```bash
# linux
./simple_cpp/build/PGRSimpleCall
# win
.\simple_cpp\build\PGRSimpleCall.exe
```

后面的内容为从0部署，仅供参考

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
./compile_zlib.sh # or .bat for windows, use Developer PowerShell for VS
./compile_pgr.sh # or .bat for windows, use Developer PowerShell for VS
./compile_simple.sh # or .bat for windows, use Developer PowerShell for VS
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
