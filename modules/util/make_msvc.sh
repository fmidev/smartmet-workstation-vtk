#!/usr/bin/bash

# Select only one of the 'cmake' lines and remove other(s), so that commands are at consecutive lines
mkdir -p ../../../VTK_ENV_MSVC \
&& cd ../../../VTK_ENV_MSVC \
&& cmake -G "Visual Studio 15 2017 Win64" -DVTK_DIR="D:/VTK_build/" ../VTK_ENV
# && cmake -G "Visual Studio 14 2015 Win64" -DVTK_DIR="D:/VTK/VTK-7.1.1_msvc/" ../fmi_VTK

read -rsp $'Press any key to continue...\n' -n 1 key