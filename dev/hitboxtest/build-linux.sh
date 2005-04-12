#!/bin/sh
export CC=gcc-2.95
export CPP=cpp-2.95
export CXX=g++-2.95
rm -R source -f
mkdir source
cd source
unzip -a ../NS-server-src.zip
cd source/curl
dos2unix *
chmod u+rwx *
./configure --build=i386-linux
make clean
make
cd ../linux
make
