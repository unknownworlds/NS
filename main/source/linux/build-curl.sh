#!/bin/sh
cd ../curl
#recode ibmpc:lat1 *
chmod u+rwx *
./configure --build=i686-linux --without-ssl --without-zlib --without-ca-bundle --with-gnu-ld --with-pic --disable-thread --disable-ftp --disable-file --disable-dict --disable-gopher --disable-telnet --disable-ipv6
make clean
make
