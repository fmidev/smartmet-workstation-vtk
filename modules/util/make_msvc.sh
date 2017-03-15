#!/usr/bin/bash

mkdir -p ../../../VTK_ENV_MSVC \
&& cd ../../../VTK_ENV_MSVC \
&& cmake -G "Visual Studio 15 2017 Win64" -DVTK_DIR="D:/VTK_build/" ../VTK_ENV

read -rsp $'Press any key to continue...\n' -n 1 key