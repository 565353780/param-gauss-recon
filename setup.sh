if [ "$(uname)" = "Darwin" ]; then
  pip install open3d==0.15.1
elif [ "$(uname)" = "Linux" ]; then
  pip install -U open3d
fi

pip install -U numpy scipy tqdm
pip install -U torch torchvision torchaudio

./compile.sh
