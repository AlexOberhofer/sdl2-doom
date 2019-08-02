#!/bin/bash

valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./linux/linuxxdoom -file ../res/DOOM.WAD
