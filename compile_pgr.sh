rm -rf bin
mkdir bin

cd ./3rd

rm -rf build
mkdir build
cd build

cmake ..
make -j

cd ../..
ln -s ./3rd/zlib/build/libz.so ./bin/libz.so
