@echo off
pscp flayra@www.jarhedz.com:/home/flayra/build/linux/ns_i386.zip ..\dlls
cd ..\dlls
c:\"program files"\Winzip\wzunzip.exe ns_i386.zip
cd ..\source
