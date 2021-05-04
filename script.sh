rm -r build
mkdir build
cd build
../init --plat pc99 --tut hello-camkes-1 --solution
ninja -j 2