cd ./3rd/zlib

rmdir /s /q build
mkdir build
cd build

cmake ..
make -j
