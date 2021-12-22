#!/usr/bin/env bash

cd src && make clean && make -j4 && cp ./sdl2-doom ../sdl2-doom && ./sdl2-doom
