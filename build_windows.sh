#!/usr/bin/env bash

cd src && make clean && make -j4 -f makefile.mingw && cp ./sdl2-doom.exe ../sdl2-doom.exe && ./sdl2-doom.exe
