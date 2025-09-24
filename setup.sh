sudo apt install libeigen3-dev -y

pip install -U numpy scipy tqdm open3d conda-pack
pip3 install torch torchvision \
  --index-url https://download.pytorch.org/whl/cu124

./compile.sh
