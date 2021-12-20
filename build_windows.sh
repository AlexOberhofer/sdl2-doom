#!/usr/bin/env bash
#NOTE: Run under a msys2-mingw64 window

cd src && make clean && make -j4 -f makefile.msys2 && ./sdl2-doom.exe
