#!/usr/bin/env bash

mkdir -p bin

g++ src/main.cpp \
src/core/Game.cpp \
src/entities/Player.cpp \
src/entities/Box.cpp \
src/physics/Collision.cpp \
src/core/TextureManager.cpp \
-o bin/six_sevenGE.exe \
$(sdl2-config --cflags --libs) -lSDL2_image

echo "Build done ✅"