#########
# SDL2 Setup Script for Github actions
# SDL2 version 2.0.18 stable
# (C) 2020 Alex Oberhofer
# GPLv2
#########

#!/usr/bin/env bash

#mingw headers
wget https://www.libsdl.org/release/SDL2-devel-2.0.18-mingw.tar.gz

tar -xvzf SDL2-devel-2.0.18-mingw.tar.gz

rm SDL2-devel-2.0.18-mingw.tar.gz

#SDL dll
wget https://www.libsdl.org/release/SDL2-2.0.18-win32-x64.zip

unzip SDL2-2.0.18-win32-x64.zip 

rm SDL2-2.0.18-win32-x64.zip 
