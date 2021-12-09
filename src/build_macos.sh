#!/bin/bash
gcc *.c -o linuxdoom -Wall -DNORMALUNIX -DMACOS -lSDL2 -lm