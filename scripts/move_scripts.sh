#!/usr/bin/bash

mkdir -p git
cp -r ../modules/gitflow/* ./git/
rm  ./git/LICENSE
rm ./git/README.md

mkdir -p cmake && cp ../modules/util/make_msvc.sh cmake/make_msvc.sh

read -rsp $'Press any key to continue...\n' -n 1 key