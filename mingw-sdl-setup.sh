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

wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-mingw.tar.gz

tar -xvzf SDL2_mixer-devel-2.0.4-mingw.tar.gz

#voodoo and this needs fixed but now required for cross compile build
cp ./SDL2_mixer-2.0.4/x86_64-w64-mingw32/include/SDL2/SDL_mixer.h ./SDL2-2.0.18/x86_64-w64-mingw32/include/SDL2/

cp ./SDL2_mixer-2.0.4/x86_64-w64-mingw32/bin/SDL2_mixer.dll ./

rm SDL2_mixer-devel-2.0.4-mingw.tar.gz

#SDL dll
wget https://www.libsdl.org/release/SDL2-2.0.18-win32-x64.zip

unzip SDL2-2.0.18-win32-x64.zip 

rm SDL2-2.0.18-win32-x64.zip 
