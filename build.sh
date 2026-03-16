#!/usr/bin/env bash
mkdir -p bin
g++ src/main.cpp -o bin/six_sevenGE.exe $(sdl2-config --cflags --libs)
echo "Build done ✅"
