rm -rf bin
mkdir bin

cd ./3rd

rm -rf build
mkdir build
cd build

cmake ..
make -j
