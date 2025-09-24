rmdir /s /q bin
mkdir bin

cd .\3rd

rmdir /s /q build
mkdir build
cd build

cmake ..
make -j

echo Build complete.
