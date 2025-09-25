rmdir /s /q bin
mkdir bin

cd .\3rd

rmdir /s /q build
mkdir build
cd build

cmake ..
nmake

cd ..\..
copy .\3rd\zlib\build\zd.dll .\bin\zd.dll
