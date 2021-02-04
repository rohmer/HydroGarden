@echo off
REM Run this file to build the project outside of the IDE.
REM WARNING: if using a different machine, copy the .rsp files together with this script.
echo GeneratePkg.cpp
C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/GeneratePkg.gcc.rsp" || exit 1
echo Linking ../../VisualGDB/Debug/GeneratePkg...
C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe @../../VisualGDB/Debug/GeneratePkg.link.rsp || exit 1
