#!/bin/bash

cd /tmp
rm /tmp/package.json
rm /tmp/HydroGarden.pkg
./CreatePkg -v 0.9.0 -n "HydroGarden.pkg" -d "HydroGarden Hydroponic Software"
./AddPkgFile -v 0.9.0 -w so -d /usr/local/bin -s /tmp
./AddPkgFile -v 0.9.0 -n /tmp/HydroGarden -d /usr/local/bin
./AddPkgFile -v 0.9.0 -w png -d /usr/local/bin -s /tmp
./GeneratePkg
