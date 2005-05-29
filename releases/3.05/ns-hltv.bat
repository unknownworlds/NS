REM Make sure you've installed and ran the "dedicated server" package to get hltv.exe
cd ../../"dedicated server"
REM hltv.exe +exec ns\ns-hltv.cfg +maxclients 256 +autorecord 1 +connect IPADDRESS:27020
hltv.exe +exec nsp\ns-hltv.cfg +maxclients 256 +autorecord 1 +connect 192.168.0.4:27016
cd ../half-life/nsp