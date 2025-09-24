#!/bin/bash

ENV_NAME='conda-amcax'

export CC=$(which gcc)
export CXX=$(which g++)
echo "Using CC: $CC"
echo "Using CXX: $CXX"

source ./3rd/${ENV_NAME}/bin/activate

rm -rf build

mkdir build
cd build
cmake \
  -DCMAKE_PREFIX_PATH=$(python3 -c 'import torch;print(torch.utils.cmake_prefix_path)') \
  ..
make -j
