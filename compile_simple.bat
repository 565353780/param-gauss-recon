cd .\simple_cpp

rmdir /s /q build
mkdir build
cd build

cmake ..
make -j
