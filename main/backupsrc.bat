@echo off (subdirs, prompt for password <-s>, date)
REM c:\"program files"\Winzip\wzzip.exe -p -r -s -ex NS-src.zip @SourceAssets.txt
c:\"program files"\Winzip\wzzip.exe -P -r -s -ex NS-full-src.zip @assets-client-source.txt @assets-shared-source.txt @assets-server-source.txt -x@assets-ignore-source.txt

